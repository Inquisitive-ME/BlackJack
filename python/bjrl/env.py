"""Gymnasium environment wrapping the C++ SimEngine.

An episode is one shoe: reset() deals a fresh shuffled shoe and step() plays
bet and play decisions until the cut card, so the deck composition (the count)
evolves within an episode. The observation's legal-action mask is exposed via
``action_masks()`` for sb3-contrib's MaskablePPO.
"""
from __future__ import annotations

import numpy as np
import gymnasium as gym
from gymnasium import spaces

from ._bjsim import SimEngine, RulesConfig, OBS_SIZE, NUM_ACTIONS, O_MASK


class BlackjackEnv(gym.Env):
    metadata = {"render_modes": []}

    def __init__(self, rules: RulesConfig | None = None, seed: int | None = None):
        super().__init__()
        self.rules = rules if rules is not None else RulesConfig()
        self._engine = SimEngine(self.rules)
        # composition/penetration/one-hots/mask/phase are in [0,1]; player
        # total/21 tops out near 1.0. A high of 2.0 leaves headroom.
        self.observation_space = spaces.Box(low=0.0, high=2.0, shape=(OBS_SIZE,), dtype=np.float32)
        self.action_space = spaces.Discrete(NUM_ACTIONS)
        self._rng = np.random.default_rng(seed)
        self._obs = np.zeros(OBS_SIZE, dtype=np.float32)

    def _shoe_seed(self) -> int:
        return int(self._rng.integers(0, 2**63 - 1))

    def reset(self, *, seed: int | None = None, options=None):
        super().reset(seed=seed)
        if seed is not None:
            self._rng = np.random.default_rng(seed)
        self._obs = np.asarray(self._engine.reset(self._shoe_seed()), dtype=np.float32)
        return self._obs, {"action_mask": self.action_masks()}

    def step(self, action):
        obs, reward, done = self._engine.step(int(action))
        self._obs = np.asarray(obs, dtype=np.float32)
        # Episode == one shoe; `done` fires at the cut card. No time-limit truncation.
        return self._obs, float(reward), bool(done), False, {"action_mask": self.action_masks()}

    def action_masks(self) -> np.ndarray:
        return self._obs[O_MASK:O_MASK + NUM_ACTIONS] > 0.5
