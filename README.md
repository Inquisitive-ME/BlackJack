# BlackJack

A fast, deterministic C++23 blackjack simulator built to be the environment for
**reinforcement-learning** research — training an agent that consumes the full
*deck history* (every card seen) to play better than classic card counting.

The core (`src/sim/`) is header-only and allocation-free on the hot path:
~120 M card deals/s and ~8.7 M full rounds/s. A Python layer (`python/`) exposes
it as a Gymnasium environment and walks through comparing RL to the High-Low
counting system.

## Build & test

Requires CMake ≥ 3.20 and a C++23 compiler (GCC 11+ / Clang 14+). GoogleTest is
found via `find_package`, or fetched (pinned) if absent.

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build            # 43 sim tests
```

## Run

```bash
./build/src/BlackJack             # interactive console game
./build/bench/bench_shoe          # deal-throughput benchmark
./build/bench/bench_engine        # full-round throughput benchmark
```

The console game is a thin front-end over the same engine the RL env uses. Ten-
valued cards all display as `10` (the engine folds 10/J/Q/K into one rank class).

## Python / RL

The pybind11 binding, Gymnasium env, and the RL-vs-counting walkthrough live in
[`python/`](python/README.md) (built with `-DBJ_BUILD_PYTHON=ON`).

## Design & roadmap

The architecture and the phased plan that produced it are in
[`docs/ROADMAP.md`](docs/ROADMAP.md). Key pieces:

- `sim/shoe.h` — pre-shuffled array + cursor; the dealt prefix *is* the deck
  history, and a revealed-card histogram gives any counting system by a dot
  product.
- `sim/sim_engine.h` — a Gymnasium-style `reset(seed)` / `step(action)` engine
  with a 35-float observation (composition + hand + dealer + action mask).
- `sim/round.h`, `sim/count_play.h` — correct round rules and count-aware play.

> The Raspberry-Pi cross-compile files (`rpi_toolchain.cmake`,
> `scripts/rpi3+Build.sh`) predate the C++23 move and need a modern toolchain to
> build; the native Linux build above is the maintained path.
