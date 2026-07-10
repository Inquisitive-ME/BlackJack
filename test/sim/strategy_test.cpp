#include "sim/basic_strategy.h"
#include "sim/cards.h"
#include "sim/hand.h"
#include "sim/round.h"
#include "sim/rules.h"
#include "sim/shoe.h"

#include "gtest/gtest.h"

#include <cstdio>

using bj::Action;
using bj::Card;
using bj::Hand;
using bj::RulesConfig;

namespace {
constexpr Card R(int rank) {
    return rank == 1 ? Card(0) : rank <= 9 ? Card(rank - 1) : Card(9);
}
Hand make(std::initializer_list<Card> cs) {
    Hand h;
    for (Card c : cs) h.add(c);
    return h;
}
} // namespace

TEST(BasicStrategy, KnownDecisions) {
    RulesConfig r;
    // Pairs (canSplit true).
    EXPECT_EQ(bj::basicStrategy(make({bj::ACE, bj::ACE}), R(6), r, false, true, false), Action::Split);
    EXPECT_EQ(bj::basicStrategy(make({R(8), R(8)}), bj::TEN, r, false, true, false), Action::Split);
    EXPECT_EQ(bj::basicStrategy(make({bj::TEN, bj::TEN}), R(6), r, false, true, false), Action::Stand);
    // Hard totals.
    EXPECT_EQ(bj::basicStrategy(make({R(7), R(4)}), R(6), r, true, false, false), Action::Double); // 11
    EXPECT_EQ(bj::basicStrategy(make({bj::TEN, R(6)}), bj::TEN, r, false, false, false), Action::Hit); // 16 v 10
    EXPECT_EQ(bj::basicStrategy(make({bj::TEN, bj::TEN}), R(6), r, false, false, false), Action::Stand); // 20
    // Soft 18 vs 9 hits (H17 multi-deck).
    EXPECT_EQ(bj::basicStrategy(make({bj::ACE, R(7)}), R(9), r, false, false, false), Action::Hit);
    // Late surrender available: 16 vs 10.
    EXPECT_EQ(bj::basicStrategy(make({bj::TEN, R(6)}), bj::TEN, r, false, false, true), Action::Surrender);
}

// The whole engine is validated end-to-end: a flat-bet basic-strategy player's
// long-run EV must land in the known house-edge band (~ -0.5%). A gross rule bug
// (mis-paid naturals, double settlements, wrong dealer play) would push it out.
TEST(BasicStrategy, HouseEdgeConvergesToKnownBand) {
    RulesConfig rules; // 6-deck, H17, 3:2, DAS, no surrender
    bj::Shoe<> shoe(rules.decks, rules.penetration, 20240710ULL);

    const long ROUNDS = 3'000'000;
    double total = 0.0;
    for (long i = 0; i < ROUNDS; ++i) {
        if (shoe.needsShuffle()) shoe.shuffle();
        total += bj::playRound(shoe, rules, bj::basicStrategy);
    }
    const double edge = total / ROUNDS; // player EV per round, in base-bet units

    std::printf("[ house-edge ] basic-strategy EV/round = %+.4f  (house edge %.3f%%)\n",
                edge, -edge * 100.0);

    EXPECT_GT(edge, -0.015) << "house edge implausibly high -- likely a rule bug";
    EXPECT_LT(edge, 0.002) << "basic strategy should not show a player advantage here";
}
