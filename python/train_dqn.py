"""End-to-end Deep Q-Network: the agent learns its OWN full strategy from reward.

No High-Low, no hand-coded rules. The agent observes only the raw 10-rank deck
composition (+ hand/dealer context) and learns, from chips won and lost, to size
its bet, take insurance, and play every hand -- it has to *discover* that a
ten-rich deck means bet big / insure / deviate.

Techniques (fixing the earlier naive DQN):
  * masked action selection AND masked targets (illegal actions never chosen),
  * Double DQN (online net picks the argmax, target net evaluates it) -> no
    overestimation blow-up,
  * dueling network (value + advantage) -> handles the many near-equal actions,
  * reward scaling, target network, long epsilon schedule,
  * TensorBoard logging incl. a periodic GREEDY-policy EV/round evaluation (the
    metric that actually matters) and a snapshot of the learned bet-vs-deck curve,
  * checkpoint + --resume so you can train for hours.

Run:
  python train_dqn.py --steps 20000000 --logdir runs/dqn1
  tensorboard --logdir runs           # watch eval/ev_per_round climb
  python train_dqn.py --steps 40000000 --logdir runs/dqn1 --resume runs/dqn1/ckpt.pt
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
REWARD_SCALE = 0.1  # chips are ~[-32,32]; scale for stable Q magnitudes (policy is scale-invariant)
NSTEP = 3           # n-step returns: bridge the delay between a bet and its round payoff


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


def greedy_ev(net, rounds=30_000, seed=777):
    """Run the greedy policy and return actual (unscaled) EV per round -- and the
    average bet placed as a function of a harness-computed true count (for a
    'is it learning to bet with the deck?' snapshot)."""
    env = F.FullBlackjackEnv()
    w = np.asarray(W_HILO)
    obs, _ = env.reset(seed=seed)
    total, n = 0.0, 0
    tc_bet = collections.defaultdict(lambda: [0.0, 0])  # tc bucket -> [sum bet, count]
    net.eval()
    with torch.no_grad():
        while n < rounds:
            mask = env.action_masks()
            qv = net(torch.from_numpy(obs.astype(np.float32)).unsqueeze(0))[0].numpy()
            a = masked_argmax(qv, mask)
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


def train(steps, logdir, resume=None, batch=256, gamma=0.99, lr=2.5e-4,
          buffer=500_000, target_sync=2_000, eval_every=500_000, eval_rounds=30_000,
          ckpt_every=500_000, eps_decay_steps=3_000_000, seed=0):
    torch.manual_seed(seed)
    random.seed(seed)
    np.random.seed(seed)
    os.makedirs(logdir, exist_ok=True)
    writer = SummaryWriter(logdir)

    net, target = DuelingQ(), DuelingQ()
    target.load_state_dict(net.state_dict())
    opt = torch.optim.Adam(net.parameters(), lr=lr)
    buf = collections.deque(maxlen=buffer)

    start_step, best_ev = 0, -1e9
    if resume and os.path.exists(resume):
        ck = torch.load(resume)
        net.load_state_dict(ck["net"]); target.load_state_dict(ck["net"])
        opt.load_state_dict(ck["opt"]); start_step = ck["step"]; best_ev = ck.get("best_ev", -1e9)
        print(f"resumed from {resume} at step {start_step:,}")

    env = F.FullBlackjackEnv()
    obs, _ = env.reset(seed=seed + 1)
    shoe_seed = seed + 2
    nbuf = collections.deque(maxlen=NSTEP)  # recent (obs, action, scaled reward)
    t0 = time.time()

    for step in range(start_step, steps):
        eps = max(0.05, 1.0 - step / eps_decay_steps)  # fixed decay -> works for "run for time"
        mask = env.action_masks()
        if random.random() < eps:
            a = int(np.random.choice(np.flatnonzero(mask)))
        else:
            with torch.no_grad():
                qv = net(torch.from_numpy(obs.astype(np.float32)).unsqueeze(0))[0].numpy()
            a = masked_argmax(qv, mask)

        nobs, r, done, _, _ = env.step(a)
        nmask = env.action_masks()
        nbuf.append((obs, a, r * REWARD_SCALE))
        # Emit the oldest transition as a full n-step return once the window fills.
        if len(nbuf) == NSTEP and not done:
            s0, a0, _ = nbuf[0]
            R = sum((gamma ** k) * nbuf[k][2] for k in range(NSTEP))
            buf.append((s0, a0, R, nobs, False, nmask))
        obs = nobs
        if done:
            # Flush the tail as terminal (truncated-return) transitions.
            m = len(nbuf)
            for i in range(m):
                si, ai, _ = nbuf[i]
                R = sum((gamma ** (k - i)) * nbuf[k][2] for k in range(i, m))
                buf.append((si, ai, R, nobs, True, nmask))
            nbuf.clear()
            obs, _ = env.reset(seed=shoe_seed); shoe_seed += 1

        if len(buf) >= batch and step % 4 == 0:
            s, act, rew, s2, term, nm = zip(*random.sample(buf, batch))
            s = torch.from_numpy(np.stack(s).astype(np.float32))
            s2 = torch.from_numpy(np.stack(s2).astype(np.float32))
            act = torch.tensor(act).unsqueeze(1)
            rew = torch.tensor(rew, dtype=torch.float32)
            term = torch.tensor(term, dtype=torch.float32)
            nm = torch.from_numpy(np.stack(nm))
            q = net(s).gather(1, act).squeeze(1)
            with torch.no_grad():
                q2_online = net(s2)
                q2_online[~nm] = -1e9
                a_star = q2_online.argmax(1, keepdim=True)         # Double DQN: online picks
                q2 = target(s2).gather(1, a_star).squeeze(1)        # target evaluates
                y = rew + (gamma ** NSTEP) * (1.0 - term) * q2
            loss = nn.functional.smooth_l1_loss(q, y)
            opt.zero_grad(); loss.backward()
            nn.utils.clip_grad_norm_(net.parameters(), 10.0)
            opt.step()

            if step % 2000 == 0:
                writer.add_scalar("train/loss", loss.item(), step)
                writer.add_scalar("train/epsilon", eps, step)
                writer.add_scalar("train/mean_q", q.mean().item(), step)

        if step % target_sync == 0:
            target.load_state_dict(net.state_dict())

        if step % eval_every == 0 and step > 0:
            ev, bet_curve = greedy_ev(net, rounds=eval_rounds)
            writer.add_scalar("eval/ev_per_round", ev, step)
            for tc, avgbet in bet_curve.items():
                writer.add_scalar(f"eval/bet_at_tc/{tc:+d}", avgbet, step)
            print(f"step {step:>10,}  eps={eps:.2f}  greedy EV/round={ev:+.4f}  "
                  f"best={best_ev:+.4f}  ({time.time()-t0:.0f}s)")
            if ev > best_ev:
                best_ev = ev
                torch.save({"net": net.state_dict(), "opt": opt.state_dict(),
                            "step": step, "best_ev": best_ev}, os.path.join(logdir, "best.pt"))

        if step % ckpt_every == 0 and step > 0:
            torch.save({"net": net.state_dict(), "opt": opt.state_dict(),
                        "step": step, "best_ev": best_ev}, os.path.join(logdir, "ckpt.pt"))

    ev, _ = greedy_ev(net, rounds=max(eval_rounds, 100_000))
    writer.add_scalar("eval/ev_per_round", ev, steps)
    print(f"\nfinal greedy EV/round = {ev:+.4f}  (best seen {best_ev:+.4f})")
    writer.close()
    return net


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--steps", type=int, default=200_000_000)  # set high; run for time, stop with Ctrl-C
    ap.add_argument("--logdir", type=str, default="runs/dqn")
    ap.add_argument("--resume", type=str, default=None)
    ap.add_argument("--eval-every", type=int, default=500_000)
    ap.add_argument("--eval-rounds", type=int, default=30_000)
    ap.add_argument("--eps-decay-steps", type=int, default=3_000_000)
    args = ap.parse_args()
    train(args.steps, args.logdir, resume=args.resume, eval_every=args.eval_every,
          eval_rounds=args.eval_rounds, eps_decay_steps=args.eps_decay_steps)
