"""End-to-end blackjack RL environment: the agent decides EVERYTHING.

Unlike `BlackjackEnv` (which only exposes bet+play via the C++ SimEngine), this
env adds the **insurance** decision and, crucially, is designed so an agent can
learn its *own* full strategy -- bet sizing, insurance, and play -- from the raw
deck composition, with **no High-Low count anywhere**. The observation hands the
agent the 10-rank remaining-composition (the deck history); it must discover for
itself that a ten-rich deck means bet big / take insurance / deviate.

Reward = actual chips won per round (bet-scaled, incl. insurance), returned when a
round settles. An episode is one shoe. The round logic mirrors the tested C++
`count_play.h`; correctness is checked in `validate_env()` below (a basic-strategy
+ count-based-bet reference reproduces the known ~+1.4%/round).

Action space (Discrete 12), gated by the legal-action mask:
  0..4  bet 1/2/4/8/16 units      (bet phase)
  5     decline insurance          (insurance phase, dealer shows Ace)
  6     take insurance
  7..11 hit / stand / double / split / surrender   (play phase)
"""
from __future__ import annotations

import numpy as np
import gymnasium as gym
from gymnasium import spaces

from ._bjsim import CountingGame, RulesConfig

# Rank buckets: 0=Ace, 1..8 = 2..9, 9 = ten-group.
VALUE = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
PER_DECK = np.array([4, 4, 4, 4, 4, 4, 4, 4, 4, 16])
ACE, TEN = 0, 9
BET_UNITS = np.array([1.0, 2.0, 4.0, 8.0, 16.0])

# Action indices.
A_BET0 = 0
A_INS_NO, A_INS_YES = 5, 6
A_HIT, A_STAND, A_DOUBLE, A_SPLIT, A_SURRENDER = 7, 8, 9, 10, 11
NUM_ACTIONS = 12

# Observation layout (39 floats).
O_COMP = 0        # [0:10]  remaining fraction per rank (the deck history)
O_PEN = 10        # penetration
O_TOTAL = 11      # player total / 21
O_SOFT = 12
O_PAIR = 13
O_UP = 14         # [14:24] dealer up-card one-hot
O_PHASE = 24      # [24:27] phase one-hot: bet, insurance, play
O_MASK = 27       # [27:39] legal-action mask
O_BET = 39        # current bet / max bet -- essential: without it the play states
                  # for a 1-unit and a 16-unit bet look identical, so the agent
                  # cannot tell which bet size is better (the reward is bet-scaled).
OBS_SIZE = 40

PH_BET, PH_INS, PH_PLAY = 0, 1, 2


def hand_hard(cards):
    return int(sum(VALUE[c] for c in cards))


def hand_total(cards):
    hard = hand_hard(cards)
    soft = any(c == ACE for c in cards) and hard + 10 <= 21
    return (hard + 10 if soft else hard), soft


def is_blackjack(cards):
    return len(cards) == 2 and hand_total(cards)[0] == 21


class Hand:
    __slots__ = ("cards", "bet", "split_ace", "surrendered", "done")

    def __init__(self, cards, bet):
        self.cards = list(cards)
        self.bet = bet
        self.split_ace = False
        self.surrendered = False
        self.done = False

    def total(self):
        return hand_total(self.cards)[0]

    def soft(self):
        return hand_total(self.cards)[1]

    def busted(self):
        return hand_hard(self.cards) > 21

    def is_pair(self):
        return len(self.cards) == 2 and self.cards[0] == self.cards[1]


class FullBlackjackEnv(gym.Env):
    metadata = {"render_modes": []}

    def __init__(self, rules: RulesConfig | None = None, seed: int | None = None):
        super().__init__()
        self.rules = rules if rules is not None else RulesConfig()
        self.full = PER_DECK * self.rules.decks
        self.total_cards = int(self.full.sum())
        self.game = CountingGame(self.rules, 0)
        self.observation_space = spaces.Box(0.0, 1.5, (OBS_SIZE,), np.float32)
        self.action_space = spaces.Discrete(NUM_ACTIONS)
        self._rng = np.random.default_rng(seed)
        self._reset_round_state()

    # ------- gym API -------
    def reset(self, *, seed=None, options=None):
        super().reset(seed=seed)
        if seed is not None:
            self._rng = np.random.default_rng(seed)
        self.game.reset(int(self._rng.integers(0, 2**63 - 1)))
        self._reset_round_state()
        self.phase = PH_BET
        return self._observe(), {}

    def step(self, action):
        action = int(action)
        if self.phase == PH_BET:
            reward, done = self._step_bet(action)
        elif self.phase == PH_INS:
            reward, done = self._step_insurance(action)
        else:
            reward, done = self._step_play(action)
        return self._observe(), float(reward), bool(done), False, {}

    def action_masks(self):
        m = np.zeros(NUM_ACTIONS, dtype=bool)
        if self.phase == PH_BET:
            m[A_BET0:A_BET0 + len(BET_UNITS)] = True
        elif self.phase == PH_INS:
            m[A_INS_NO] = m[A_INS_YES] = True
        else:
            m[A_HIT] = m[A_STAND] = True
            cd, cs, csu = self._play_legality()
            if cd:
                m[A_DOUBLE] = True
            if cs:
                m[A_SPLIT] = True
            if csu:
                m[A_SURRENDER] = True
        return m

    # ------- round machinery -------
    def _reset_round_state(self):
        self.hands, self.cur, self.dealer, self.hole = [], 0, [], 0
        self.up, self.bet, self.insurance, self.splits = 0, 1.0, 0.0, 0
        self.player_bj = self.dealer_bj = False

    def _deal_round(self, bet):
        self.bet = bet
        self.splits = 0
        self.cur = 0
        p1 = self.game.deal_up()
        self.up = self.game.deal_up()
        p2 = self.game.deal_up()
        self.hole = self.game.deal_hole()
        self.hands = [Hand([p1, p2], bet)]
        self.dealer = [self.up]
        self.player_bj = is_blackjack([p1, p2])
        self.dealer_bj = (self.up in (ACE, TEN)) and is_blackjack([self.up, self.hole])

    def _finish_round(self, reward):
        # Back to the bet phase for the next round; `done` (shoe hit the cut card)
        # signals the episode is over and the caller will reset.
        done = self.game.needs_shuffle()
        self.phase = PH_BET
        return reward, done

    def _resolve_naturals(self):
        """Called once naturals are known (after any insurance). Returns (reward,
        settled) where settled=True if the round ended on a natural."""
        if self.player_bj or self.dealer_bj:
            self.game.reveal(self.hole)
            if self.player_bj and self.dealer_bj:
                r = 0.0
            elif self.player_bj:
                r = self.rules.blackjack_pays * self.bet
            else:
                r = -self.bet
            return r + self.insurance, True
        return 0.0, False

    def _step_bet(self, action):
        idx = action - A_BET0
        bet = BET_UNITS[idx] if 0 <= idx < len(BET_UNITS) else BET_UNITS[0]
        self._deal_round(bet)
        self.insurance = 0.0
        if self.up == ACE and not self.player_bj:
            self.phase = PH_INS
            return 0.0, False
        r, settled = self._resolve_naturals()
        if settled:
            return self._finish_round(r)
        self.phase = PH_PLAY
        return 0.0, False

    def _step_insurance(self, action):
        if action == A_INS_YES:
            stake = self.bet / 2.0
            self.insurance = 2.0 * stake if self.dealer_bj else -stake
        else:
            self.insurance = 0.0
        r, settled = self._resolve_naturals()
        if settled:
            return self._finish_round(r)
        self.phase = PH_PLAY
        return 0.0, False

    def _play_legality(self):
        ph = self.hands[self.cur]
        first = len(ph.cards) == 2
        original = self.splits == 0
        can_split = (first and ph.is_pair() and self.splits < self.rules.max_splits
                     and (ph.cards[0] != ACE or self.rules.resplit_aces or self.splits == 0))
        can_double = first and (original or self.rules.double_after_split)
        can_surr = self.rules.surrender_allowed and original and first and len(self.hands) == 1
        return can_double, can_split, can_surr

    def _advance(self):
        while self.cur < len(self.hands) and self.hands[self.cur].done:
            self.cur += 1

    def _step_play(self, action):
        ph = self.hands[self.cur]
        cd, cs, csu = self._play_legality()
        if action == A_SPLIT and cs:
            pc = ph.cards[0]
            ph.cards = [pc, self.game.deal_up()]
            new = Hand([pc, self.game.deal_up()], ph.bet)
            new.split_ace = ph.split_ace = (pc == ACE)
            new.done = ph.done = (pc == ACE)  # split aces: one card, stand
            self.hands.insert(self.cur + 1, new)
            self.splits += 1
        elif action == A_DOUBLE and cd:
            ph.bet *= 2.0
            ph.cards.append(self.game.deal_up())
            ph.done = True
        elif action == A_SURRENDER and csu:
            ph.surrendered = True
            ph.done = True
        elif action == A_HIT:
            ph.cards.append(self.game.deal_up())
            if ph.busted():
                ph.done = True
        else:  # stand / masked
            ph.done = True

        self._advance()
        if self.cur < len(self.hands):
            return 0.0, False
        return self._finish_round(self._showdown())

    def _showdown(self):
        self.game.reveal(self.hole)
        self.dealer.append(self.hole)
        any_live = any(not h.surrendered and not h.busted() for h in self.hands)
        if any_live:
            dt, ds = hand_total(self.dealer)
            while dt < 17 or (dt == 17 and ds and self.rules.hit_soft_17):
                self.dealer.append(self.game.deal_up())
                dt, ds = hand_total(self.dealer)
        dtotal = hand_total(self.dealer)[0]
        dbust = hand_hard(self.dealer) > 21
        reward = self.insurance
        for h in self.hands:
            if h.surrendered:
                reward += -0.5 * h.bet
            elif h.busted():
                reward += -h.bet
            elif dbust or h.total() > dtotal:
                reward += h.bet
            elif h.total() < dtotal:
                reward += -h.bet
        return reward

    # ------- observation -------
    def _observe(self):
        o = np.zeros(OBS_SIZE, dtype=np.float32)
        seen = self.game.seen()
        o[O_COMP:O_COMP + 10] = 1.0 - seen / self.full          # remaining fraction per rank
        o[O_PEN] = 1.0 - self.game.cards_remaining() / self.total_cards
        if self.phase == PH_PLAY and self.cur < len(self.hands):
            ph = self.hands[self.cur]
            o[O_TOTAL] = ph.total() / 21.0
            o[O_SOFT] = 1.0 if ph.soft() else 0.0
            o[O_PAIR] = 1.0 if ph.is_pair() else 0.0
        if self.phase in (PH_INS, PH_PLAY):
            o[O_UP + self.up] = 1.0
            o[O_BET] = self.bet / BET_UNITS[-1]      # the bet is now committed; expose it
        o[O_PHASE + self.phase] = 1.0
        o[O_MASK:O_MASK + NUM_ACTIONS] = self.action_masks()
        return o


def validate_env(rounds=2_000_000, seed=1):
    """Sanity check: drive the env with basic strategy + a count-based bet using a
    High-Low count computed *by the test harness* (not the agent). Should reproduce
    the known player edge, proving the env's round logic is correct."""
    from ._bjsim import basic_action, W_HILO
    w = np.asarray(W_HILO)
    env = FullBlackjackEnv()
    obs, _ = env.reset(seed=seed)
    total, n = 0.0, 0
    while n < rounds:
        m = env.action_masks()
        if env.phase == PH_BET:
            seen = env.game.seen()
            rc = float((seen * w).sum())
            dr = env.game.decks_remaining()
            tc = rc / dr if dr > 1e-9 else 0.0
            bet_idx = 0 if tc < 1 else 1 if tc < 2 else 2 if tc < 3 else 3 if tc < 4 else 4
            a = A_BET0 + bet_idx
            n += 1
        elif env.phase == PH_INS:
            seen = env.game.seen()
            dr = env.game.decks_remaining()
            tc = float((seen * w).sum()) / dr if dr > 1e-9 else 0.0
            a = A_INS_YES if tc >= 3 else A_INS_NO
        else:
            ph = env.hands[env.cur]
            pc = ph.cards[0] if ph.is_pair() else 0
            act = basic_action(ph.total(), ph.soft(), pc, env.up, bool(m[A_DOUBLE]),
                               bool(m[A_SPLIT]), bool(m[A_SURRENDER]))
            a = [A_HIT, A_STAND, A_DOUBLE, A_SPLIT, A_SURRENDER][act]
            if not m[a]:
                a = A_STAND
        obs, r, done, _, _ = env.step(a)
        total += r
        if done:
            obs, _ = env.reset(seed=seed + n)
    return total / n
