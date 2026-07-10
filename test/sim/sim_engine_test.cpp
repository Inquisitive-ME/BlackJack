#include "sim/basic_strategy.h"
#include "sim/rules.h"
#include "sim/sim_engine.h"

#include "gtest/gtest.h"

#include <cstdio>
#include <utility>

using bj::Action;
using bj::Observation;
using bj::Phase;
using bj::RulesConfig;
using bj::SimEngine;

namespace {

// Play one full shoe (episode) with a flat minimum bet and basic strategy.
// Returns (total reward, rounds played).
std::pair<double, int> playShoe(SimEngine &eng, std::uint64_t seed, const RulesConfig &rules) {
    eng.reset(seed);
    double total = 0.0;
    bool done = false;
    while (!done) {
        int action;
        if (eng.phase() == Phase::Bet) {
            action = 0; // flat 1-unit bet
        } else {
            bool cd, cs, csu;
            eng.legality(cd, cs, csu);
            Action a = bj::basicStrategy(eng.currentHand(), eng.dealerUp(), rules, cd, cs, csu);
            action = SimEngine::playActionIndex(a);
        }
        auto res = eng.step(action);
        total += res.reward;
        done = res.done;
    }
    return {total, eng.rounds()};
}

} // namespace

TEST(SimEngine, ResetGivesBetPhase) {
    SimEngine eng;
    Observation obs = eng.reset(7);
    EXPECT_EQ(eng.phase(), Phase::Bet);
    EXPECT_EQ(obs.x[bj::O_PHASE], 0.0f);
    for (int i = 0; i < bj::BET_LEVELS; ++i) EXPECT_EQ(obs.x[bj::O_MASK + i], 1.0f);
    for (int i = bj::A_HIT; i < bj::NUM_ACTIONS; ++i) EXPECT_EQ(obs.x[bj::O_MASK + i], 0.0f);
    for (int b = 0; b < bj::NUM_BUCKETS; ++b) EXPECT_EQ(obs.x[bj::O_COMP + b], 0.0f);
    EXPECT_EQ(obs.x[bj::O_PEN], 0.0f);
}

TEST(SimEngine, BetLeadsToPlayWithHitStandLegal) {
    SimEngine eng;
    for (std::uint64_t s = 1; s <= 50; ++s) {
        eng.reset(s);
        auto res = eng.step(0); // bet 1 unit
        if (eng.phase() == Phase::Play) {
            EXPECT_EQ(res.obs.x[bj::O_PHASE], 1.0f);
            EXPECT_EQ(res.obs.x[bj::O_MASK + bj::A_HIT], 1.0f);
            EXPECT_EQ(res.obs.x[bj::O_MASK + bj::A_STAND], 1.0f);
            int ones = 0;
            for (int i = 0; i < bj::NUM_BUCKETS; ++i) ones += (res.obs.x[bj::O_UP + i] == 1.0f);
            EXPECT_EQ(ones, 1); // exactly one dealer up-card one-hot
            return;
        }
    }
    FAIL() << "no non-natural opening round found in 50 seeds";
}

TEST(SimEngine, CompositionAndPenetrationGrow) {
    SimEngine eng;
    RulesConfig rules;
    eng.reset(3);
    Observation obs{};
    for (int round = 0; round < 10; ++round) {
        auto res = eng.step(0);
        obs = res.obs;
        while (eng.phase() == Phase::Play) {
            bool cd, cs, csu;
            eng.legality(cd, cs, csu);
            Action a = bj::basicStrategy(eng.currentHand(), eng.dealerUp(), rules, cd, cs, csu);
            res = eng.step(SimEngine::playActionIndex(a));
            obs = res.obs;
        }
        if (res.done) break;
    }
    EXPECT_GT(obs.x[bj::O_PEN], 0.0f);
    float compSum = 0.0f;
    for (int b = 0; b < bj::NUM_BUCKETS; ++b) compSum += obs.x[bj::O_COMP + b];
    EXPECT_GT(compSum, 0.0f);
}

TEST(SimEngine, EpisodeTerminates) {
    SimEngine eng;
    RulesConfig rules;
    auto [reward, rounds] = playShoe(eng, 99, rules);
    EXPECT_GT(rounds, 0);
    EXPECT_EQ(eng.phase(), Phase::Done);
    (void)reward;
}

TEST(SimEngine, Deterministic) {
    RulesConfig rules;
    SimEngine e1(rules), e2(rules);
    auto [r1, n1] = playShoe(e1, 12345, rules);
    auto [r2, n2] = playShoe(e2, 12345, rules);
    EXPECT_EQ(r1, r2);
    EXPECT_EQ(n1, n2);
}

// The stepped engine must reproduce the reference playRound() EV: flat-bet basic
// strategy converges to the same ~-0.5% band. This is the golden check that the
// resumable state machine and the one-shot resolver agree.
TEST(SimEngine, HouseEdgeMatchesReference) {
    RulesConfig rules;
    SimEngine eng(rules);
    double total = 0.0;
    long rounds = 0;
    std::uint64_t seed = 1;
    while (rounds < 2'000'000) {
        auto [r, n] = playShoe(eng, seed++, rules);
        total += r;
        rounds += n;
    }
    double edge = total / rounds;
    std::printf("[ sim-engine ] flat-bet basic-strategy EV/round = %+.4f  (house edge %.3f%%)\n",
                edge, -edge * 100.0);
    EXPECT_GT(edge, -0.015);
    EXPECT_LT(edge, 0.002);
}
