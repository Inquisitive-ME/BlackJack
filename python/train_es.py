"""Train a betting count that beats High-Low, with an evolution strategy (CEM).

High-Low is very close to the ceiling for a *linear* betting count, so the win
over it is small and easily drowned by Monte-Carlo noise. Two design choices make
the search work:

  * Objective = the PAIRED difference in EV vs a ramp-tuned High-Low, measured on
    the SAME shoes (common random numbers). The shared round outcome cancels, so
    the difference has tiny variance and CEM can resolve a ~0.001/round edge.
  * The candidate learns its 10 count weights AND its bet ramp; weights are
    normalized to High-Low's magnitude so the comparison is about count quality.

The learned count typically rediscovers a Wong-Halves-like weighting (fractional
weights, 5 heaviest) that edges out High-Low.

Run:  python train_es.py            # ~10 min, writes artifacts/learned_policy.npz
"""
from __future__ import annotations

import os
import numpy as np

from bjrl import baselines as B
from bjrl._bjsim import RulesConfig, W_HILO, evaluate_two_linear_policies

SPREAD = (1.0, 12.0)
MAG = float(np.sum(np.abs(np.asarray(W_HILO))))  # 7
ROUNDS = 30_000_000          # per fitness eval (paired -> low variance)
CRN_SEED = 777               # common random numbers: stable, deterministic landscape
ARTIFACTS = os.path.join(os.path.dirname(__file__), "artifacts")


def normalize(weights):
    w = np.asarray(weights, dtype=float)
    s = np.sum(np.abs(w))
    return w * (MAG / s) if s > 1e-6 else w


def paired_diff(params, opp_w, opp_a, opp_b, rounds=ROUNDS, seed=CRN_SEED):
    w = normalize(params[:10])
    a, b = float(params[10]), float(params[11])
    r = evaluate_two_linear_policies(RulesConfig(), [float(x) for x in w], a, b,
                                     [float(x) for x in opp_w], float(opp_a), float(opp_b),
                                     SPREAD[0], SPREAD[1], int(rounds), int(seed))
    return r["ev_diff"]  # candidate - High-Low; maximize


def train(generations=12, pop=18, elite_frac=0.3, verbose=True):
    # Ramp-tuned High-Low is the opponent to beat.
    h = B.tune_hilo(spread=SPREAD, rounds=6_000_000, seed=7)
    opp_w, opp_a, opp_b = np.asarray(W_HILO, dtype=float), h["a"], h["b"]
    if verbose:
        print(f"opponent High-Low ramp a={opp_a:+.1f} b={opp_b:+.1f}  EV/round={h['ev_per_round']:+.4f}")

    rng = np.random.default_rng(0)
    mean = np.concatenate([opp_w, [opp_a, opp_b]])
    sigma = np.concatenate([np.full(10, 0.45), [0.6, 0.6]])
    n_elite = max(2, int(pop * elite_frac))

    for g in range(generations):
        cands = mean + sigma * rng.standard_normal((pop, 12))
        fits = np.array([paired_diff(c, opp_w, opp_a, opp_b) for c in cands])
        order = np.argsort(fits)[-n_elite:]
        elite = cands[order]
        mean = elite.mean(axis=0)
        sigma = np.maximum(elite.std(axis=0), 0.02)
        if verbose:
            print(f"gen {g:2d}  best_diff={fits.max():+.5f}  elite_diff={fits[order].mean():+.5f}  "
                  f"ramp=({mean[10]:+.2f},{mean[11]:+.2f})")
    return mean, (opp_w, opp_a, opp_b)


if __name__ == "__main__":
    os.makedirs(ARTIFACTS, exist_ok=True)
    params, (opp_w, opp_a, opp_b) = train()
    weights = normalize(params[:10])
    a, b = float(params[10]), float(params[11])

    # Validate on a large held-out paired sample (different seed).
    val = evaluate_two_linear_policies(RulesConfig(), [float(x) for x in weights], a, b,
                                       [float(x) for x in opp_w], float(opp_a), float(opp_b),
                                       SPREAD[0], SPREAD[1], 400_000_000, seed=999)
    d, se = val["ev_diff"], val["stderr_diff"]
    print("\nlearned weights (norm):", np.round(weights, 3))
    print(f"learned ramp: a={a:+.3f} b={b:+.3f}")
    print(f"held-out paired diff (learned - High-Low) = {d:+.5f} +/- {1.96*se:.5f} (95% CI)")
    print(f"  learned EV/round = {val['ev_a']:+.5f}   High-Low EV/round = {val['ev_b']:+.5f}")

    np.savez(os.path.join(ARTIFACTS, "learned_policy.npz"),
             weights=weights, ramp_a=a, ramp_b=b,
             hilo_a=opp_a, hilo_b=opp_b, spread=np.asarray(SPREAD))
    print("saved", os.path.join(ARTIFACTS, "learned_policy.npz"))
