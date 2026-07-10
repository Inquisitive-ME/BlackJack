"""Compare betting/play strategies and produce the walkthrough's headline result.

Three players, all 6-deck H17 at a fixed 1-12 bet spread:
  1. No counting        -- flat 1-unit bet, basic-strategy play  (the house wins)
  2. Standard counter   -- High-Low bet ramp,   basic-strategy play
  3. Count-aware player  -- High-Low bet ramp,   count-based play deviations + insurance

(2) shows counting flipping the edge to the player; (3) shows that using the deck
composition for PLAY (not just betting) beats the standard counter -- the payoff of
the full deck-history observation.

Also reports the betting-count experiment: an RL/ES agent that learns its own count
weights (train_es.py) essentially ties High-Low, which is near the linear ceiling.

Outputs: artifacts/summary.txt and artifacts/comparison.png.

Run (after train_es.py):  python compare.py
"""
from __future__ import annotations

import os
import numpy as np

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

from bjrl import baselines as B
from bjrl._bjsim import RulesConfig, CountingGame, W_HILO, evaluate_counting

SPREAD = (1.0, 12.0)
ART = os.path.join(os.path.dirname(__file__), "artifacts")
LABELS = ["A", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
INSURANCE_TC = 3.0
N_EVAL = 400_000_000


def ramp_bet(tc, a, b, lo=SPREAD[0], hi=SPREAD[1]):
    return np.clip(np.floor(a + b * tc + 0.5), lo, hi)


def simulate(wh, ah, bh, wl, al, bl, rounds, seed):
    g = CountingGame(RulesConfig(), seed)
    wh, wl = list(map(float, wh)), list(map(float, wl))
    tc_h = np.empty(rounds); bet_h = np.empty(rounds); bet_l = np.empty(rounds)
    for i in range(rounds):
        if g.needs_shuffle():
            g.shuffle()
        tc_h[i] = g.true_count(wh)
        bet_h[i] = ramp_bet(tc_h[i], ah, bh)
        bet_l[i] = ramp_bet(g.true_count(wl), al, bl)
        g.bet_and_play(1.0)
    return tc_h, bet_h, bet_l


def mean_bet_by_count(tc, bet, edges):
    xs, ys = [], []
    for lo, hi in zip(edges[:-1], edges[1:]):
        m = (tc >= lo) & (tc < hi)
        if m.sum() > 50:
            xs.append((lo + hi) / 2)
            ys.append(bet[m].mean())
    return np.array(xs), np.array(ys)


def main():
    r = RulesConfig()
    hilo = np.asarray(W_HILO, dtype=float)

    data = np.load(os.path.join(ART, "learned_policy.npz"))
    wl, al, bl = data["weights"], float(data["ramp_a"]), float(data["ramp_b"])
    ah, bh = (float(data["hilo_a"]), float(data["hilo_b"])) if "hilo_a" in data else (-3.5, 8.5)

    print("scoring the three players ...")
    w = list(map(float, hilo))
    nocount = evaluate_counting(r, w, 1.0, 0.0, 1, 1, False, INSURANCE_TC, N_EVAL, 20240710)   # flat bet
    std     = evaluate_counting(r, w, ah, bh, 1, 12, False, INSURANCE_TC, N_EVAL, 20240710)     # HiLo + basic
    aware   = evaluate_counting(r, w, ah, bh, 1, 12, True,  INSURANCE_TC, N_EVAL, 20240710)      # HiLo + deviations

    gain = aware["ev_per_round"] - std["ev_per_round"]
    gain_se = (std["stderr"] ** 2 + aware["stderr"] ** 2) ** 0.5

    # Betting-count experiment (secondary): learned linear count vs High-Low (paired).
    print("paired betting-count comparison ...")
    pair = B.paired(wl, al, bl, hilo, ah, bh, rounds=300_000_000, seed=999)  # A=learned, B=High-Low
    bdiff, bse = pair["ev_diff"], pair["stderr_diff"]

    def pct(x):
        return f"{x*100:+.2f}%"

    L = []
    L.append("Blackjack strategy comparison  (6-deck H17, 1-12 bet spread, per initial-bet unit)")
    L.append("=" * 82)
    L.append(f"1. No counting   (flat bet, basic play)     EV/round = {nocount['ev_per_round']:+.5f}  ({pct(nocount['ev_per_round'])})   <- house edge")
    L.append(f"2. Standard count(High-Low bet, basic play) EV/round = {std['ev_per_round']:+.5f}  ({pct(std['ev_per_round'])})   <- counting flips it to a PLAYER edge")
    L.append(f"3. Count-aware   (High-Low bet, deviations) EV/round = {aware['ev_per_round']:+.5f}  ({pct(aware['ev_per_round'])})   <- +insurance & index plays")
    L.append("-" * 82)
    L.append(f"Play-deviation gain (3 - 2): {gain:+.5f} +/- {1.96*gain_se:.5f}  (z={gain/gain_se:.1f}),  {100*gain/std['ev_per_round']:+.0f}% relative EV")
    L.append("  -> using the deck composition for PLAY (not just betting) beats the standard counter.")
    L.append("")
    L.append("Betting-count experiment (secondary): an ES agent learns its own count weights.")
    verdict = "beats" if bdiff - 1.96 * bse > 0 else ("ties" if bdiff + 1.96 * bse > 0 else "below")
    L.append(f"  learned linear count {verdict} High-Low: paired diff {bdiff:+.5f} +/- {1.96*bse:.5f} (300M shared shoes)")
    L.append("  High-Low is near the ceiling for a LINEAR betting count, so the margin is tiny;")
    L.append("  the learned weights (below) rediscover a High-Low-like counting system from reward.")
    L.append("    rank:     " + " ".join(f"{x:>5}" for x in LABELS))
    L.append("    High-Low: " + " ".join(f"{x:>5.1f}" for x in hilo))
    L.append("    learned:  " + " ".join(f"{x:>5.1f}" for x in wl))
    summary = "\n".join(L)
    print("\n" + summary + "\n")
    with open(os.path.join(ART, "summary.txt"), "w") as f:
        f.write(summary + "\n")

    # ---- plots ----
    print("simulating for plots ...")
    tc_h, bet_h, bet_l = simulate(hilo, ah, bh, wl, al, bl, rounds=300_000, seed=2024)
    fig, ax = plt.subplots(1, 3, figsize=(16, 4.6))

    names = ["No counting\n(flat, basic)", "Standard count\n(HiLo, basic)", "Count-aware\n(HiLo, deviations)"]
    evs = [nocount["ev_per_round"], std["ev_per_round"], aware["ev_per_round"]]
    errs = [1.96 * nocount["stderr"], 1.96 * std["stderr"], 1.96 * aware["stderr"]]
    colors = ["#c0392b", "#2980b9", "#27ae60"]
    ax[0].bar(names, [e * 100 for e in evs], yerr=[e * 100 for e in errs], color=colors, capsize=4)
    ax[0].axhline(0, color="k", lw=0.8)
    ax[0].set_ylabel("EV per round (%)")
    ax[0].set_title("Edge: counting, then count-aware play")
    for i, e in enumerate(evs):
        ax[0].text(i, e * 100 + (0.1 if e >= 0 else -0.2), f"{e*100:+.2f}%", ha="center", fontsize=9)

    edges = np.arange(-6, 8.01, 1.0)
    xh, yh = mean_bet_by_count(tc_h, bet_h, edges)
    ax[1].plot(xh, yh, "-o", color="#2980b9", ms=4)
    ax[1].set_title("Bet size vs true count (High-Low)")
    ax[1].set_xlabel("true count"); ax[1].set_ylabel("avg bet (units)")

    x = np.arange(10)
    ax[2].bar(x - 0.2, hilo, width=0.4, label="High-Low", color="#2980b9")
    ax[2].bar(x + 0.2, wl, width=0.4, label="RL-learned", color="#8e44ad")
    ax[2].set_xticks(x); ax[2].set_xticklabels(LABELS)
    ax[2].axhline(0, color="k", lw=0.6)
    ax[2].set_title("Learned count weights rediscover High-Low")
    ax[2].set_xlabel("card"); ax[2].set_ylabel("weight"); ax[2].legend()

    fig.tight_layout()
    out = os.path.join(ART, "comparison.png")
    fig.savefig(out, dpi=110)
    print("saved", out)


if __name__ == "__main__":
    main()
