"""Deep Q-Network for blackjack PLAY -- the neural counterpart to train_mc_play.py.

A small masked DQN learns the five play actions (hit/stand/double/split/surrender)
directly from the 35-float observation (which includes the deck composition, so
the net can in principle learn count-dependent deviations). Betting is fixed to a
flat 1 unit during training so the play signal is clean; the reward is chips won
per round, bootstrapped back through each hand's decisions with a masked TD target.

Honest expectation: like the tabular learner, this recovers basic-strategy-quality
play and the deviation structure, but the per-deviation EV is tiny and near the
estimation-noise floor, so its net EV lands around basic play -- not dramatically
above it. The hand-designed Illustrious-18 (+37%, see compare.py) is the ceiling.

Run:  python train_dqn.py --steps 600000        # ~5-8 min on CPU
"""
from __future__ import annotations

import argparse
import collections
import random
import time
import numpy as np
import torch
import torch.nn as nn

import bjrl
from bjrl import _bjsim as m

RULES = bjrl.RulesConfig()
OBS = bjrl.OBS_SIZE
O_MASK, O_PEN = m.O_MASK, 10
NA = 5  # play actions: hit, stand, double, split, surrender
FULL = np.array([4] * 9 + [16], dtype=np.float32) * RULES.decks
RC_COEF = FULL * np.asarray(bjrl.W_HILO, dtype=np.float32)


class QNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.net = nn.Sequential(nn.Linear(OBS, 128), nn.ReLU(),
                                 nn.Linear(128, 128), nn.ReLU(),
                                 nn.Linear(128, NA))

    def forward(self, x):
        return self.net(x)


def play_mask(obs):
    return obs[O_MASK + 5:O_MASK + 10] > 0.5


def obs_true_count(obs):
    dr = RULES.decks * (1.0 - obs[O_PEN])
    return float(obs[0:10] @ RC_COEF) / dr if dr > 1e-9 else 0.0


def count_bet_action(tc):
    """Discrete SimEngine bet level (0..4 = 1,2,4,8,16 units) from the true count.
    Aggressive enough that a basic-strategy counter shows a player edge."""
    return 0 if tc < 1 else 1 if tc < 2 else 2 if tc < 3 else 3 if tc < 4 else 4


def train(steps, seed=0, batch=128, gamma=0.97, lr=5e-4):
    torch.manual_seed(seed)
    random.seed(seed)
    eng = bjrl.SimEngine(RULES)
    q, qt = QNet(), QNet()
    qt.load_state_dict(q.state_dict())
    opt = torch.optim.Adam(q.parameters(), lr=lr)
    buf = collections.deque(maxlen=100_000)

    shoe_seed = 1
    obs = np.asarray(eng.reset(shoe_seed), dtype=np.float32)
    t0 = time.time()
    for step in range(steps):
        if eng.is_done():
            shoe_seed += 1
            obs = np.asarray(eng.reset(shoe_seed), dtype=np.float32)
            continue
        if eng.is_bet_phase():
            o2, _, done = eng.step(0)  # flat 1-unit bet
            obs = np.asarray(o2, dtype=np.float32)
            continue

        mask = play_mask(obs)
        legal = np.flatnonzero(mask)
        eps = max(0.05, 1.0 - step / (0.8 * steps))
        if random.random() < eps:
            a = int(random.choice(legal))
        else:
            with torch.no_grad():
                qv = q(torch.from_numpy(obs)).numpy()
            qv[~mask] = -1e9
            a = int(qv.argmax())

        o2, r, done = eng.step(5 + a)
        o2 = np.asarray(o2, dtype=np.float32)
        settled = done or eng.is_bet_phase()
        nmask = np.zeros(NA, dtype=bool) if settled else play_mask(o2)
        buf.append((obs, a, np.float32(r), o2, settled, nmask))
        obs = o2

        if len(buf) >= batch and step % 4 == 0:
            s, act, rew, s2, term, nm = zip(*random.sample(buf, batch))
            s = torch.from_numpy(np.stack(s))
            s2 = torch.from_numpy(np.stack(s2))
            act = torch.tensor(act, dtype=torch.int64).unsqueeze(1)
            rew = torch.tensor(rew)
            term = torch.tensor(term, dtype=torch.float32)
            nm = torch.from_numpy(np.stack(nm))
            q_sa = q(s).gather(1, act).squeeze(1)
            with torch.no_grad():
                q2 = qt(s2)
                q2[~nm] = -1e9
                max_next = q2.max(1).values
                target = rew + gamma * (1.0 - term) * max_next
            loss = nn.functional.smooth_l1_loss(q_sa, target)
            opt.zero_grad()
            loss.backward()
            opt.step()

        if step % 20_000 == 0:
            qt.load_state_dict(q.state_dict())
        if step % 100_000 == 0 and step:
            print(f"  step {step:>8,}  eps={eps:.2f}  ({time.time()-t0:.0f}s)")
    return q


def basic_policy(obs, mask):
    total = int(round(obs[11] * 21))
    soft, pair = obs[12] > 0.5, obs[13] > 0.5
    up = int(np.argmax(obs[14:24]))
    if pair:
        pc = 0 if (soft and total == 12) else (9 if total // 2 == 10 else total // 2 - 1)
    else:
        pc = 0
    a = m.basic_action(total, bool(soft), pc, up, bool(mask[2]), bool(mask[3]), bool(mask[4]))
    return a if mask[a] else (1 if mask[1] else 0)


def dqn_policy(q):
    def pol(obs, mask):
        with torch.no_grad():
            qv = q(torch.from_numpy(obs.astype(np.float32))).numpy()
        qv[~mask] = -1e9
        return int(qv.argmax())
    return pol


def evaluate(policy_fn, rounds=400_000, seed=90210):
    """Play `rounds` rounds with a count-based discrete bet ramp; `policy_fn`
    chooses play actions. Returns EV per round (same framework for every policy)."""
    eng = bjrl.SimEngine(RULES)
    total, n, shoe_seed = 0.0, 0, seed
    obs = np.asarray(eng.reset(shoe_seed), dtype=np.float32)
    while n < rounds:
        if eng.is_done():
            shoe_seed += 1
            obs = np.asarray(eng.reset(shoe_seed), dtype=np.float32)
            continue
        if eng.is_bet_phase():
            obs, r, _ = eng.step(count_bet_action(obs_true_count(obs)))
            n += 1
        else:
            a = policy_fn(obs, play_mask(obs))
            obs, r, _ = eng.step(5 + a)
        obs = np.asarray(obs, dtype=np.float32)
        total += r
    return total / n


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--steps", type=int, default=600_000)
    args = ap.parse_args()

    print(f"Training masked DQN for {args.steps:,} play steps ...")
    q = train(args.steps)

    print("\nEvaluating (count-based discrete bet ramp, same for every policy):")
    n_eval = 800_000
    basic_ev = evaluate(basic_policy, n_eval)
    dqn_ev = evaluate(dqn_policy(q), n_eval)
    print(f"  basic play : EV/round = {basic_ev:+.4f}")
    print(f"  DQN  play  : EV/round = {dqn_ev:+.4f}")
    print(f"  difference : {dqn_ev - basic_ev:+.4f}/round "
          f"({'DQN ahead' if dqn_ev > basic_ev else 'basic ahead'})")
