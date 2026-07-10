"""bjrl -- fast blackjack simulation for RL, with a deck-history observation.

Thin Python layer over the C++ core (compiled as ``_bjsim`` next to this file).
"""
from ._bjsim import (  # noqa: F401
    RulesConfig,
    SimEngine,
    CountingGame,
    evaluate_linear_policy,
    OBS_SIZE,
    NUM_ACTIONS,
    BET_LEVELS,
    O_MASK,
    O_PHASE,
    A_HIT,
    A_STAND,
    A_DOUBLE,
    A_SPLIT,
    A_SURRENDER,
    W_HILO,
)

from .env import BlackjackEnv  # noqa: F401

__all__ = [
    "RulesConfig",
    "SimEngine",
    "CountingGame",
    "evaluate_linear_policy",
    "BlackjackEnv",
    "OBS_SIZE",
    "NUM_ACTIONS",
    "BET_LEVELS",
    "O_MASK",
    "O_PHASE",
    "W_HILO",
]
