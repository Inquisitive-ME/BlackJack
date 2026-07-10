#include "sim/hand.h"

#include "gtest/gtest.h"

using bj::Card;
using bj::Hand;

// Bucket helpers: rank r (2..9) -> bucket r-1; ACE=0; TEN group=9.
static constexpr Card R(int rank) {
    return rank == 1 ? Card(0) : rank <= 9 ? Card(rank - 1) : Card(9);
}

TEST(Hand, HardTotalSums) {
    Hand h;
    h.add(R(5));
    h.add(R(6));
    EXPECT_EQ(h.total(), 11);
    EXPECT_FALSE(h.isSoft());
    EXPECT_FALSE(h.isBusted());
    EXPECT_EQ(h.numCards(), 2);
}

TEST(Hand, SoftAceThenHardensToAvoidBust) {
    Hand h;
    h.add(bj::ACE); // 11
    h.add(R(6));    // soft 17
    EXPECT_EQ(h.total(), 17);
    EXPECT_TRUE(h.isSoft());

    h.add(R(5)); // 1 + 6 + 5 = 12; promoting the ace would bust -> hard 12
    EXPECT_EQ(h.total(), 12);
    EXPECT_FALSE(h.isSoft());
    EXPECT_FALSE(h.isBusted());
}

TEST(Hand, Blackjack) {
    Hand h;
    h.add(bj::ACE);
    h.add(bj::TEN);
    EXPECT_EQ(h.total(), 21);
    EXPECT_TRUE(h.isBlackjack());
    EXPECT_FALSE(h.isBusted());
}

TEST(Hand, TwentyOneWithThreeCardsIsNotBlackjack) {
    Hand h;
    h.add(R(7));
    h.add(R(7));
    h.add(R(7));
    EXPECT_EQ(h.total(), 21);
    EXPECT_FALSE(h.isBlackjack());
    EXPECT_FALSE(h.isBusted());
}

TEST(Hand, Bust) {
    Hand h;
    h.add(bj::TEN);
    h.add(bj::TEN);
    h.add(R(5));
    EXPECT_EQ(h.total(), 25);
    EXPECT_TRUE(h.isBusted());
}

TEST(Hand, PairDetection) {
    Hand eights;
    eights.add(R(8));
    eights.add(R(8));
    EXPECT_TRUE(eights.isPair());
    EXPECT_EQ(eights.pairCard(), R(8));

    Hand notPair;
    notPair.add(R(8));
    notPair.add(R(9));
    EXPECT_FALSE(notPair.isPair());
}

TEST(Hand, AnyTwoTenValuedCardsArePair) {
    // 10 and King both fold into the ten-group, so they split as a pair.
    Hand h;
    h.add(R(10));
    h.add(R(13));
    EXPECT_TRUE(h.isPair());
    EXPECT_EQ(h.pairCard(), bj::TEN);
}

TEST(Hand, AcesPromoteOnlyOnce) {
    Hand h;
    h.add(bj::ACE);
    h.add(bj::ACE); // 11 + 1 = 12, soft, splittable pair
    EXPECT_EQ(h.total(), 12);
    EXPECT_TRUE(h.isSoft());
    EXPECT_TRUE(h.isPair());

    h.add(bj::ACE); // 11 + 1 + 1 = 13
    EXPECT_EQ(h.total(), 13);
    EXPECT_TRUE(h.isSoft());

    h.add(R(9)); // A,A,A,9 = 1+1+1+9 = 12; promoting any ace would bust -> hard 12
    EXPECT_EQ(h.total(), 12);
    EXPECT_FALSE(h.isSoft());
    EXPECT_FALSE(h.isBusted());
}

TEST(Hand, ClearResets) {
    Hand h;
    h.add(bj::TEN);
    h.add(R(7));
    h.clear();
    EXPECT_EQ(h.total(), 0);
    EXPECT_EQ(h.numCards(), 0);
    EXPECT_FALSE(h.isSoft());
    EXPECT_FALSE(h.isBusted());
    EXPECT_FALSE(h.isPair());
}
