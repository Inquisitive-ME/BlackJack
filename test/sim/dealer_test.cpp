#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/shoe.h"

#include "gtest/gtest.h"

using bj::dealerPlay;
using bj::dealerShouldHit;
using bj::Hand;
using bj::Shoe;

TEST(Dealer, HitsBelow17) {
    EXPECT_TRUE(dealerShouldHit(12, false, false));
    EXPECT_TRUE(dealerShouldHit(16, false, true));
}

TEST(Dealer, StandsOnHard17AndAbove) {
    EXPECT_FALSE(dealerShouldHit(17, false, false));
    EXPECT_FALSE(dealerShouldHit(17, false, true));
    EXPECT_FALSE(dealerShouldHit(18, false, true));
    EXPECT_FALSE(dealerShouldHit(21, false, true));
}

TEST(Dealer, Soft17DependsOnRule) {
    // Soft 17 (e.g. Ace + 6): stand under S17, hit under H17.
    EXPECT_FALSE(dealerShouldHit(17, /*soft=*/true, /*hitSoft17=*/false)); // S17: stand
    EXPECT_TRUE(dealerShouldHit(17, /*soft=*/true, /*hitSoft17=*/true));   // H17: hit
    // Soft 18+ always stands.
    EXPECT_FALSE(dealerShouldHit(18, true, true));
}

// dealerPlay must always stop in a terminal state: busted, or a total in
// [17,21] where the policy says stand -- for both rules and many seeds.
TEST(Dealer, PlayAlwaysReachesTerminalState) {
    for (bool h17 : {false, true}) {
        for (std::uint64_t seed = 0; seed < 200; ++seed) {
            Shoe<> shoe(6, 0.75, seed);
            Hand d;
            d.add(shoe.dealFaceUp());
            d.add(shoe.dealFaceUp());
            dealerPlay(d, shoe, h17);

            EXPECT_FALSE(dealerShouldHit(d.total(), d.isSoft(), h17))
                << "seed " << seed << " h17 " << h17;
            EXPECT_TRUE(d.isBusted() || (d.total() >= 17 && d.total() <= 21))
                << "seed " << seed << " total " << d.total();
        }
    }
}

// Under S17 the dealer stands the moment it holds a soft 17; under H17 it must
// draw at least once more from that state.
TEST(Dealer, S17StandsOnSoft17WhileH17Draws) {
    // Construct a fresh shoe and a soft-17 hand, then check the immediate decision.
    Hand soft17;
    soft17.add(bj::ACE);
    soft17.add(bj::Card(5)); // bucket 5 == rank 6  -> Ace + 6 = soft 17
    ASSERT_EQ(soft17.total(), 17);
    ASSERT_TRUE(soft17.isSoft());

    EXPECT_FALSE(dealerShouldHit(soft17.total(), soft17.isSoft(), /*h17=*/false));
    EXPECT_TRUE(dealerShouldHit(soft17.total(), soft17.isSoft(), /*h17=*/true));
}
