"""Evaluation helpers for linear betting policies.

A *policy* here is: play every hand with basic strategy, and size the bet from a
running count.  The bet is::

    true_count = dot(seen_counts, weights) / decks_remaining
    bet        = clip(round(ramp_a + ramp_b * true_count), bet_min, bet_max)

High-Low is this with ``weights = W_HILO``; the learned policy uses its own
``weights`` (and folds the ramp slope into their scale, so ``ramp_b = 1``).
Everything is scored inside the C++ core for speed.
"""
from __future__ import annotations

import numpy as np

from ._bjsim import (
    RulesConfig,
    W_HILO,
    evaluate_linear_policy,
    evaluate_two_linear_policies,
)

DEFAULT_SPREAD = (1.0, 12.0)  # fixed for every strategy -> we compare count quality, not aggression


def _w(weights) -> list[float]:
    return [float(x) for x in np.asarray(weights, dtype=float)]


def evaluate(weights, ramp_a, ramp_b=1.0, rules=None, spread=DEFAULT_SPREAD, rounds=5_000_000, seed=1):
    rules = rules or RulesConfig()
    return evaluate_linear_policy(rules, _w(weights), float(ramp_a), float(ramp_b),
                                  float(spread[0]), float(spread[1]), int(rounds), int(seed))


def tune_hilo(rules=None, spread=DEFAULT_SPREAD, rounds=3_000_000, seed=7):
    """Grid-tune the High-Low bet ramp (a + b*TC) within the fixed spread.

    Returns the best stats dict augmented with the chosen ``a`` and ``b``.
    """
    rules = rules or RulesConfig()
    w = _w(W_HILO)
    best = None
    for a in np.arange(-4.0, 2.01, 0.5):
        for b in np.arange(0.5, 9.01, 0.5):
            s = evaluate_linear_policy(rules, w, float(a), float(b),
                                       float(spread[0]), float(spread[1]), int(rounds), int(seed))
            if best is None or s["ev_per_round"] > best["ev_per_round"]:
                best = dict(s)
                best["a"], best["b"] = float(a), float(b)
    return best


def paired(weights_a, a_a, b_a, weights_b, a_b, b_b,
           rules=None, spread=DEFAULT_SPREAD, rounds=200_000_000, seed=999):
    """Paired (common-random-numbers) comparison of two policies on shared shoes."""
    rules = rules or RulesConfig()
    return evaluate_two_linear_policies(rules, _w(weights_a), float(a_a), float(b_a),
                                        _w(weights_b), float(a_b), float(b_b),
                                        float(spread[0]), float(spread[1]), int(rounds), int(seed))
