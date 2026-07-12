"""End-to-end Deep Q-Network: the agent learns its OWN full strategy from reward.

No High-Low, no hand-coded rules. The agent observes only the raw 10-rank deck
composition (+ hand/dealer context) and learns, from chips won and lost, to size
its bet, take insurance, and play every hand -- it has to *discover* that a
ten-rich deck means bet big / insure / deviate.

Two design fixes make it actually converge (the naive version got stuck far below
basic strategy -- it never even learned to double, because the play signal was
buried under bet-size variance):

  1. PLAY-FIRST CURRICULUM (--play-steps). For the first phase the bet is forced
     to a flat 1 unit, so a play decision carries +/-1-2 units of variance instead
     of +/-32 (a 16x reduction). The agent learns to play (double, split, hit,
     stand, insure) cleanly; then betting is unlocked.

  2. CONTROL-VARIATE REWARD. reward = chips - E[basic-strategy result on the SAME
     starting hand]. The baseline is a precomputed table keyed on the round's
     initial state, so it is subtracted equally across every play action -- it
     CANNOT bias which play is best; it only cancels the "which hand did I get
     dealt" variance, exposing the "did I play/bet it better than basic" signal.
     This is the same common-random-numbers trick that made train_es.py work.

Plus the usual DQN hygiene: masked action selection AND masked targets, Double DQN
(no Q-overestimation blow-up), a dueling network (the actions here have very close
values), reward scaling, a target network, a long epsilon schedule, n-step returns,
TensorBoard logging (incl. a periodic GREEDY play-EV and full-EV evaluation and the
learned bet-vs-count curve), and checkpoint + --resume for multi-hour runs.

Run:
  python train_dqn.py --steps 60000000 --logdir runs/dqn1
  tensorboard --logdir runs           # watch eval/play_ev climb to ~ -0.006 (basic), then eval/ev_per_round
  python train_dqn.py --logdir runs/dqn1 --resume runs/dqn1/ckpt.pt
"""
from __future__ import annotations

import argparse
import collections
import os
import random
import time
import numpy as np
import torch
import torch.nn as nn
from torch.utils.tensorboard import SummaryWriter

import bjrl.full_env as F
from bjrl._bjsim import W_HILO

OBS = F.OBS_SIZE
NA = F.NUM_ACTIONS
REWARD_SCALE = 0.5   # advantage rewards are small & centered; scale up for usable Q magnitudes
NSTEP = 3            # n-step returns: bridge the (short) gap between a decision and the round payoff
BASELINE_PATH = "artifacts/play_baseline.npz"


# ----------------------------------------------------------------------------
# Control-variate baseline: E[basic-strategy round result | initial state].
# Keyed on the round's opening (player total, soft, pair, pair-card, dealer up).
# ----------------------------------------------------------------------------
def init_key(cards, up):
    total, soft = F.hand_total(cards)
    pair = cards[0] == cards[1]
    return (int(total), int(soft), int(pair), int(cards[0]) if pair else -1, int(up))


def build_baseline(rounds=4_000_000, seed=0):
    """Drive the env with PURE basic strategy, flat 1-unit bet, insurance always
    declined; average the round result per opening-state key. Any function of the
    opening state is a valid variance-reduction baseline; this one makes the reward
    read 'how much better than basic did I do', whose optimum is optimal play."""
    from bjrl._bjsim import basic_action
    env = F.FullBlackjackEnv()
    obs, _ = env.reset(seed=seed)
    s = collections.defaultdict(float)
    c = collections.defaultdict(int)
    key = None
    n = 0
    t0 = time.time()
    while n < rounds:
        ph = env.phase
        m = env.action_masks()
        if ph == F.PH_BET:
            a = F.A_BET0
        elif ph == F.PH_INS:
            a = F.A_INS_NO
        else:
            h = env.hands[env.cur]
            pc = h.cards[0] if h.is_pair() else 0
            act = basic_action(h.total(), h.soft(), pc, env.up,
                               bool(m[F.A_DOUBLE]), bool(m[F.A_SPLIT]), bool(m[F.A_SURRENDER]))
            a = [F.A_HIT, F.A_STAND, F.A_DOUBLE, F.A_SPLIT, F.A_SURRENDER][act]
            if not m[a]:
                a = F.A_STAND
        was_bet = ph == F.PH_BET
        obs, r, done, _, _ = env.step(a)
        if was_bet:
            key = init_key(env.hands[0].cards, env.up)
            n += 1
        if env.phase == F.PH_BET:          # round settled
            s[key] += r
            c[key] += 1
        if done:
            obs, _ = env.reset(seed=seed + n)
    table = {k: s[k] / c[k] for k in c}
    print(f"built baseline: {len(table)} opening states over {n:,} rounds "
          f"({time.time()-t0:.0f}s)", flush=True)
    return table


def save_baseline(table, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    items = list(table.items())
    np.savez(path,
             keys=np.array([k for k, _ in items], dtype=np.int64),
             vals=np.array([v for _, v in items], dtype=np.float64))


def load_baseline(path):
    d = np.load(path)
    return {tuple(int(x) for x in k): float(v) for k, v in zip(d["keys"], d["vals"])}


def get_baseline(path=BASELINE_PATH, rounds=4_000_000):
    if os.path.exists(path):
        return load_baseline(path)
    table = build_baseline(rounds)
    save_baseline(table, path)
    return table


# ----------------------------------------------------------------------------
class DuelingQ(nn.Module):
    def __init__(self, obs=OBS, n_act=NA, hidden=512):
        super().__init__()
        self.feat = nn.Sequential(nn.Linear(obs, hidden), nn.ReLU(),
                                  nn.Linear(hidden, hidden), nn.ReLU())
        self.value = nn.Linear(hidden, 1)
        self.adv = nn.Linear(hidden, n_act)

    def forward(self, x):
        f = self.feat(x)
        a = self.adv(f)
        return self.value(f) + a - a.mean(dim=1, keepdim=True)


def masked_argmax(qvals: np.ndarray, mask: np.ndarray) -> int:
    q = qvals.copy()
    q[~mask] = -1e9
    return int(q.argmax())


def _greedy_action(net, obs, mask):
    with torch.no_grad():
        qv = net(torch.from_numpy(obs.astype(np.float32)).unsqueeze(0))[0].numpy()
    return masked_argmax(qv, mask)


def greedy_play_ev(net, rounds=40_000, seed=999):
    """PLAY quality: greedy play + greedy insurance, but a forced flat 1-unit bet
    so betting can't confound it. Compare to basic strategy ~ -0.006."""
    env = F.FullBlackjackEnv()
    obs, _ = env.reset(seed=seed)
    total, n, dbl, plays = 0.0, 0, 0, 0
    net.eval()
    while n < rounds:
        mask = env.action_masks()
        ph = env.phase
        a = F.A_BET0 if ph == F.PH_BET else _greedy_action(net, obs, mask)
        if ph == F.PH_BET:
            n += 1
        elif ph == F.PH_PLAY:
            plays += 1
            dbl += (a == F.A_DOUBLE)
        obs, r, done, _, _ = env.step(a)
        total += r
        if done:
            obs, _ = env.reset(seed=seed + n)
    net.train()
    return total / n, (dbl / max(plays, 1))


def greedy_ev(net, rounds=40_000, seed=777):
    """FULL policy (its own bets + insurance + play): actual EV/round, plus the
    average bet placed vs a harness-computed true count."""
    env = F.FullBlackjackEnv()
    w = np.asarray(W_HILO)
    obs, _ = env.reset(seed=seed)
    total, n = 0.0, 0
    tc_bet = collections.defaultdict(lambda: [0.0, 0])
    net.eval()
    while n < rounds:
        mask = env.action_masks()
        a = _greedy_action(net, obs, mask)
        if env.phase == F.PH_BET:
            dr = env.game.decks_remaining()
            tc = float((env.game.seen() * w).sum()) / dr if dr > 1e-9 else 0.0
            b = tc_bet[int(round(np.clip(tc, -5, 9)))]
            b[0] += F.BET_UNITS[a - F.A_BET0] if a < F.A_BET0 + len(F.BET_UNITS) else 1.0
            b[1] += 1
            n += 1
        obs, r, done, _, _ = env.step(a)
        total += r
        if done:
            obs, _ = env.reset(seed=seed + n)
    net.train()
    bet_curve = {k: v[0] / v[1] for k, v in sorted(tc_bet.items()) if v[1] > 30}
    return total / n, bet_curve


def train(steps, logdir, resume=None, play_steps=8_000_000, batch=256, gamma=0.99,
          lr=2.5e-4, buffer=500_000, target_sync=2_000, eval_every=250_000,
          eval_rounds=40_000, ckpt_every=500_000, eps_decay_steps=3_000_000,
          baseline_path=BASELINE_PATH, seed=0):
    torch.manual_seed(seed)
    random.seed(seed)
    np.random.seed(seed)
    os.makedirs(logdir, exist_ok=True)
    writer = SummaryWriter(logdir)

    baseline = get_baseline(baseline_path)
    print(f"baseline: {len(baseline)} opening states, play_steps={play_steps:,}", flush=True)

    net, target = DuelingQ(), DuelingQ()
    target.load_state_dict(net.state_dict())
    opt = torch.optim.Adam(net.parameters(), lr=lr)
    buf = collections.deque(maxlen=buffer)

    start_step, best = 0, -1e9
    if resume and os.path.exists(resume):
        ck = torch.load(resume)
        net.load_state_dict(ck["net"]); target.load_state_dict(ck["net"])
        opt.load_state_dict(ck["opt"]); start_step = ck["step"]; best = ck.get("best", -1e9)
        print(f"resumed from {resume} at step {start_step:,}", flush=True)

    env = F.FullBlackjackEnv()
    obs, _ = env.reset(seed=seed + 1)
    shoe_seed = seed + 2
    nbuf = collections.deque(maxlen=NSTEP)
    t0 = time.time()
    last_loss = 0.0
    hands = 0
    round_key, round_bet = None, 1.0
    hb_reward, hb_hands, hb_dbl, hb_play = 0.0, 0, 0, 0

    for step in range(start_step, steps):
        eps = max(0.05, 1.0 - step / eps_decay_steps)
        stage1 = step < play_steps
        cur_phase = env.phase
        mask = env.action_masks()
        if random.random() < eps:
            a = int(np.random.choice(np.flatnonzero(mask)))
        else:
            a = _greedy_action(net, obs, mask)
        if stage1 and cur_phase == F.PH_BET:
            a = F.A_BET0                       # curriculum: flat 1-unit bet while learning to play
        if cur_phase == F.PH_PLAY:
            hb_play += 1
            hb_dbl += (a == F.A_DOUBLE)

        nobs, r, done, _, _ = env.step(a)
        nmask = env.action_masks()
        if cur_phase == F.PH_BET:
            round_key = init_key(env.hands[0].cards, env.up)
            round_bet = env.bet
        settled = env.phase == F.PH_BET
        r_learn = r - baseline.get(round_key, 0.0) * round_bet if settled else r

        hb_reward += r
        hands += (cur_phase == F.PH_BET)
        hb_hands += (cur_phase == F.PH_BET)
        nbuf.append((obs, a, r_learn * REWARD_SCALE))
        if len(nbuf) == NSTEP and not done:
            s0, a0, _ = nbuf[0]
            R = sum((gamma ** k) * nbuf[k][2] for k in range(NSTEP))
            buf.append((s0, a0, R, nobs, False, nmask))
        obs = nobs
        if done:
            m = len(nbuf)
            for i in range(m):
                si, ai, _ = nbuf[i]
                R = sum((gamma ** (k - i)) * nbuf[k][2] for k in range(i, m))
                buf.append((si, ai, R, nobs, True, nmask))
            nbuf.clear()
            obs, _ = env.reset(seed=shoe_seed); shoe_seed += 1

        if len(buf) >= batch and step % 4 == 0:
            sb, act, rew, s2, term, nm = zip(*random.sample(buf, batch))
            sb = torch.from_numpy(np.stack(sb).astype(np.float32))
            s2 = torch.from_numpy(np.stack(s2).astype(np.float32))
            act = torch.tensor(act).unsqueeze(1)
            rew = torch.tensor(rew, dtype=torch.float32)
            term = torch.tensor(term, dtype=torch.float32)
            nm = torch.from_numpy(np.stack(nm))
            q = net(sb).gather(1, act).squeeze(1)
            with torch.no_grad():
                q2_online = net(s2)
                q2_online[~nm] = -1e9
                a_star = q2_online.argmax(1, keepdim=True)
                q2 = target(s2).gather(1, a_star).squeeze(1)
                y = rew + (gamma ** NSTEP) * (1.0 - term) * q2
            loss = nn.functional.smooth_l1_loss(q, y)
            opt.zero_grad(); loss.backward()
            nn.utils.clip_grad_norm_(net.parameters(), 10.0)
            opt.step()
            last_loss = loss.item()
            if step % 2000 == 0:
                writer.add_scalar("train/loss", last_loss, step)
                writer.add_scalar("train/epsilon", eps, step)
                writer.add_scalar("train/mean_q", q.mean().item(), step)

        if step % target_sync == 0:
            target.load_state_dict(net.state_dict())

        if step % 20_000 == 0 and step > 0:
            print(f"step {step:>10,} | hands {hands:>9,} | {step/(time.time()-t0):.0f} st/s | "
                  f"{'PLAY ' if stage1 else 'BET+'} | eps {eps:.2f} | loss {last_loss:.3f} | "
                  f"dbl {hb_dbl/max(hb_play,1):.1%} | reward/hand {hb_reward/max(hb_hands,1):+.3f}",
                  flush=True)
            hb_reward, hb_hands, hb_dbl, hb_play = 0.0, 0, 0, 0

        if step % eval_every == 0 and step > 0:
            play_ev, dbl_rate = greedy_play_ev(net, rounds=eval_rounds)
            full_ev, bet_curve = greedy_ev(net, rounds=eval_rounds)
            writer.add_scalar("eval/play_ev", play_ev, step)
            writer.add_scalar("eval/play_double_rate", dbl_rate, step)
            writer.add_scalar("eval/ev_per_round", full_ev, step)
            for tc, avgbet in bet_curve.items():
                writer.add_scalar(f"eval/bet_at_tc/{tc:+d}", avgbet, step)
            metric = play_ev if stage1 else full_ev
            print(f"step {step:>10,}  play_ev={play_ev:+.4f} (dbl {dbl_rate:.1%}, basic ~ -0.006)  "
                  f"full_ev={full_ev:+.4f}  best={best:+.4f}  ({time.time()-t0:.0f}s)", flush=True)
            if metric > best:
                best = metric
                torch.save({"net": net.state_dict(), "opt": opt.state_dict(),
                            "step": step, "best": best}, os.path.join(logdir, "best.pt"))

        if step % ckpt_every == 0 and step > 0:
            torch.save({"net": net.state_dict(), "opt": opt.state_dict(),
                        "step": step, "best": best}, os.path.join(logdir, "ckpt.pt"))

    play_ev, _ = greedy_play_ev(net, rounds=max(eval_rounds, 100_000))
    full_ev, _ = greedy_ev(net, rounds=max(eval_rounds, 100_000))
    print(f"\nfinal play_ev={play_ev:+.4f}  full_ev={full_ev:+.4f}  (best {best:+.4f})", flush=True)
    writer.close()
    return net


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--steps", type=int, default=200_000_000)   # run for time; stop with Ctrl-C
    ap.add_argument("--logdir", type=str, default="runs/dqn")
    ap.add_argument("--resume", type=str, default=None)
    ap.add_argument("--play-steps", type=int, default=8_000_000)  # flat-bet play curriculum length
    ap.add_argument("--eval-every", type=int, default=250_000)
    ap.add_argument("--eval-rounds", type=int, default=40_000)
    ap.add_argument("--eps-decay-steps", type=int, default=3_000_000)
    ap.add_argument("--lr", type=float, default=2.5e-4)
    ap.add_argument("--gamma", type=float, default=0.99)
    ap.add_argument("--build-baseline", action="store_true", help="build+cache the baseline table and exit")
    args = ap.parse_args()
    if args.build_baseline:
        save_baseline(build_baseline(), BASELINE_PATH)
    else:
        train(args.steps, args.logdir, resume=args.resume, play_steps=args.play_steps,
              eval_every=args.eval_every, eval_rounds=args.eval_rounds,
              eps_decay_steps=args.eps_decay_steps, lr=args.lr, gamma=args.gamma)
