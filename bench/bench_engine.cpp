// Throughput of the full RL step loop: flat-bet basic-strategy rounds/sec.
// Always -O2 -march=native regardless of CMAKE_BUILD_TYPE.
#include "sim/basic_strategy.h"
#include "sim/rules.h"
#include "sim/sim_engine.h"

#include <chrono>
#include <cstdint>
#include <cstdio>

int main() {
    bj::RulesConfig rules;
    bj::SimEngine eng(rules);
    const long TARGET_ROUNDS = 20'000'000;
    long rounds = 0;
    double total = 0.0;
    std::uint64_t seed = 1;

    auto t0 = std::chrono::steady_clock::now();
    eng.reset(seed++);
    while (rounds < TARGET_ROUNDS) {
        int action;
        if (eng.phase() == bj::Phase::Bet) {
            action = 0; // flat 1-unit bet
            ++rounds;
        } else {
            bool cd, cs, csu;
            eng.legality(cd, cs, csu);
            bj::Action a = bj::basicStrategy(eng.currentHand(), eng.dealerUp(), rules, cd, cs, csu);
            action = bj::SimEngine::playActionIndex(a);
        }
        auto res = eng.step(action);
        total += res.reward;
        if (res.done) eng.reset(seed++);
    }
    auto t1 = std::chrono::steady_clock::now();

    double secs = std::chrono::duration<double>(t1 - t0).count();
    std::printf("%ld rounds in %.3f s  ->  %.2f M rounds/s  (EV/round %+.4f)\n",
                rounds, secs, rounds / secs / 1e6, total / rounds);
    return 0;
}
