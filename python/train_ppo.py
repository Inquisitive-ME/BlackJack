"""Deep-RL example: MaskablePPO on the full bet+play Gymnasium env.

This is the standard Gymnasium + Stable-Baselines3 path -- it learns the *whole*
policy (bet sizing AND every play decision) from scratch, using the observation's
legal-action mask. It shows that the env drops straight into off-the-shelf deep
RL.

Note: matching (let alone beating) a hand-tuned High-Low counter with end-to-end
deep RL takes a lot of training. This script is a runnable starting point, not a
finished agent. For the rigorous betting comparison that *beats* High-Low, see
train_es.py / compare.py.

Run:  python train_ppo.py --timesteps 300000
"""
from __future__ import annotations

import argparse

from sb3_contrib import MaskablePPO
from sb3_contrib.common.wrappers import ActionMasker

from bjrl import BlackjackEnv


def _mask(env):
    return env.action_masks()


def make_env():
    return ActionMasker(BlackjackEnv(), _mask)


def evaluate(model, episodes=300, seed=10_000):
    env = make_env()
    total_reward, total_rounds = 0.0, 0
    for e in range(episodes):
        obs, _ = env.reset(seed=seed + e)
        done = False
        while not done:
            action, _ = model.predict(obs, action_masks=env.action_masks(), deterministic=True)
            obs, r, term, trunc, _ = env.step(int(action))
            total_reward += r
            done = term or trunc
        total_rounds += env.unwrapped._engine.rounds()
    return total_reward / max(total_rounds, 1), total_rounds


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--timesteps", type=int, default=300_000)
    args = ap.parse_args()

    model = MaskablePPO("MlpPolicy", make_env(), verbose=1,
                        n_steps=4096, batch_size=512, gamma=0.999)
    model.learn(total_timesteps=args.timesteps)
    ev, rounds = evaluate(model)
    print(f"\nMaskablePPO EV/round over {rounds} rounds: {ev:+.4f} "
          f"(compare to High-Low ~+0.013 at a 1-12 spread)")
