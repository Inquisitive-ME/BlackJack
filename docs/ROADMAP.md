# Structural roadmap: BlackJack → fast RL simulation library

Goal: an **extremely fast, deterministic** blackjack simulator to serve as the RL
environment for training a card-counting agent that consumes the full **deck
history** (every card seen so far) and learns strategies stronger than classic
counting (High-Low / Zen / Omega-II).

## Decisions (locked)

- **Architecture:** grow a new POD-based core in `src/sim/` next to the legacy
  `card`/`deck`/`BJHand` classes; the existing gtest suite stays green the whole
  way. Once the core is solid, port the CLI onto it and **delete the legacy code**,
  folding its coverage into the new tests.
- **Consumer / binding:** design a clean **C ABI + observation contract** now so
  either a C++ trainer or Python (pybind11 / ctypes → numpy) can bind later. Build
  the C++ loop first; add the Python binding when needed.
- **Ruleset:** a **configurable `RulesConfig`** (decks, penetration, H17/S17,
  3:2 vs 6:5, DAS, max splits, surrender, insurance) so the agent can train across
  variants.

## Invariants held after every phase

- `ctest` is green.
- No console/DB I/O in core simulation code.
- One seed reproduces a rollout bit-for-bit.
- A `bench` target guards throughput against regressions.

## Why the fold-to-10-buckets model

The 10-bucket rank histogram — `0 = Ace, 1..8 = ranks 2..9, 9 = ten-group` — is the
**minimal sufficient statistic** for blackjack EV and for every classic count; any
counting system is a fixed weight-vector **dot product** over it. So the agent gets a
fixed-size observation regardless of penetration, and it can *learn* a weighting that
strictly subsumes High-Low/Zen/Omega-II. Order of past cards is irrelevant to EV given
composition (relevant only for non-uniform shuffle research — and that history is free
anyway via the dealt prefix).

---

## Phases

### Phase 0 — Scaffolding & guardrails · S
- `.gitignore`; new `bjsim` core target under `src/sim/`; `test/sim/` + `bench/`
  wired into CMake. The new core does not link SQLite (the dead DB stays only on the
  frozen legacy path until Phase 6).
- Seed the existing nondeterministic deck/random tests via `seedRandom()`.
- **Verify:** build + ctest green; core + bench compile.

### Phase 1 — `Card` + `Shoe` (the deck-history mechanism) · M
- `using Card = uint8_t` (bucket 0..9) + `constexpr` tables (`VALUE`, `PER_DECK`,
  `W_HILO`/`W_ZEN`/`W_OMEGA`).
- `Shoe`: fixed array + `cursor_` + `seen_[10]` + `full_[10]` + its own seedable
  engine; one Fisher-Yates per shuffle; `dealFaceUp` / `dealHole` / `reveal`;
  `needsShuffle` at the round boundary; `runningCount` / `trueCount` / `history()`.
  This replaces the legacy `std::function` flip-callback and *retains* the history.
- **Verify:** shoe-conservation invariant; same-seed reproducibility; count tables
  pinned against legacy `card`; balanced counts net zero over a full shoe;
  **bench ≥ 100M deals/s** (Release).

### Phase 2 — Incremental `Hand` + dealer policy · S–M
- `Hand` with O(1) incremental total + soft-ace tracking (kills the O(n²) recompute);
  `isBlackjack`/`isBusted`/pair detection. Dealer as a fixed policy driven by
  `RulesConfig.hitSoft17` (no polymorphism).
- **Verify:** total/soft/pair/blackjack edge-case units; dealer H17/S17 stopping.

### Phase 3 — `RulesConfig` + correct rules as pure functions · M–L
- The `RulesConfig` struct. Reimplement the round as pure, testable logic, fixing
  every correctness bug found in review: surrender terminal (no double-settle);
  dealer-BJ peek; one-card-terminal double; split deals the 2nd card to each hand /
  right-hand gating / DAS / max-splits / split-aces; natural pays 3:2 and pushes vs
  dealer natural; insurance pays 2:1; **illegal actions masked, never looped**.
- **Verify:** one unit per rule + a house-edge statistical test (fixed basic-strategy
  policy converges into the known edge band for the configured rules, seeded).

### Phase 4 — `SimEngine`: reset / step / observation / reward · L
- `reset(seed) → Observation`; `step(action) → (obs, reward, done, info)`. The 30-float
  observation (remaining-fraction[10] = compressed deck history, penetration, hand
  context, dealer up-card one-hot, legal-action mask, phase). **Betting is a
  first-class action.** Reward = net chips per settled hand. Zero I/O.
- **Verify:** scripted basic-strategy agent runs K deterministic episodes; golden
  obs/reward trace for a fixed seed; mean reward ≈ house edge; `step` never returns an
  action outside the mask; **bench: hands/s** (target 1–10M+/s Release).

### Phase 5 — C ABI (+ optional Python) · M
- `extern "C"` `create/reset/step/free` → `float[30]` + reward + flags. Optional
  pybind11/ctypes example returning numpy arrays, behind a CMake option (off by default).
- **Verify:** C smoke test drives an episode; if built, a Python script steps the env.

### Phase 6 — Converge: port CLI, delete legacy, finalize · M
- Rewrite the console game as a thin adapter over `SimEngine`. Delete
  `card`/`deck`/`hand`/`bj_hand`/`player_impl`/`dealer_impl`/`ai_interface` + the
  superseded legacy tests (coverage folded into `test/sim`). Finalize CMake: `bjsim`
  is the core, `BlackJack` CLI links it, SQLite/Python optional; drop or isolate the
  RPi apparatus and `includeGUnit.cmake`.
- **Verify:** full build + ctest green on the new suite only; CLI plays a round;
  `bench` reports final hands/s.

## Dependency flow

```
P0 ─▶ P1 ─▶ P2 ─▶ P3 ─▶ P4 ─▶ P5
                         └────▶ P6 (needs P4; folds in P1–P3)
```

Critical path to a usable RL env: P0→P1→P2→P3→P4. P5/P6 are packaging + cleanup.

## Known tradeoffs

- Two implementations coexist during P1–P5 (accepted; legacy is *frozen*, not edited).
- The 10-bucket fold drops suit and which-ten identity: lossless for play/EV/counts,
  but the ported CLI can't print *which* ten was dealt.
