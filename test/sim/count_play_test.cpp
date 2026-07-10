#include "sim/count_play.h"
#include "sim/hand.h"
#include "sim/rules.h"

#include "gtest/gtest.h"

#include <vector>

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
struct StackedShoe {
    std::vector<Card> cards;
    std::size_t pos = 0;
    Card dealFaceUp() { return cards[pos++]; }
    Card dealHole() { return cards[pos++]; }
    void reveal(Card) {}
};
} // namespace

TEST(CountPlay, SixteenVsTenIsCountDependent) {
    RulesConfig r;
    Hand h16 = make({bj::TEN, R(6)}); // hard 16
    // Basic strategy hits 16 vs 10; the deviation stands at true count >= 0.
    EXPECT_EQ(bj::countAwareStrategy(h16, bj::TEN, r, false, false, false, -1.0), Action::Hit);
    EXPECT_EQ(bj::countAwareStrategy(h16, bj::TEN, r, false, false, false, +1.0), Action::Stand);
}

TEST(CountPlay, TwelveVsThreeStandsOnlyAtHighCount) {
    RulesConfig r;
    Hand h12 = make({R(10), R(2)}); // hard 12 (ten + 2)
    EXPECT_EQ(bj::countAwareStrategy(h12, R(3), r, false, false, false, 0.0), Action::Hit);   // basic
    EXPECT_EQ(bj::countAwareStrategy(h12, R(3), r, false, false, false, 3.0), Action::Stand); // deviation
}

TEST(CountPlay, InsuranceIsTakenAndPaysWhenDealerHasBlackjack) {
    RulesConfig r;
    // Player 10,6 ; dealer Ace up, ten hole => dealer blackjack.
    StackedShoe shoe{{bj::TEN, bj::ACE, R(6), bj::TEN}};
    // With deviations and a true count above the insurance threshold, insurance
    // (+2:1 on half the bet) exactly offsets the lost main bet -> net 0.
    double withIns = bj::playRoundCounting(shoe, r, /*tc=*/5.0, /*deviations=*/true,
                                           /*insuranceTC=*/3.0, /*bet=*/1.0);
    EXPECT_DOUBLE_EQ(withIns, 0.0);

    StackedShoe shoe2{{bj::TEN, bj::ACE, R(6), bj::TEN}};
    // Basic play never insures: the round just loses the main bet.
    double noIns = bj::playRoundCounting(shoe2, r, /*tc=*/5.0, /*deviations=*/false,
                                         /*insuranceTC=*/3.0, /*bet=*/1.0);
    EXPECT_DOUBLE_EQ(noIns, -1.0);
}

TEST(CountPlay, DeviationsMatchBasicWhenCountIsNeutral) {
    // At true count 0 with no insurable ace, count-aware play should equal basic
    // play on a plain hand (12 vs 6: both stand).
    RulesConfig r;
    Hand h = make({R(10), R(2)});
    EXPECT_EQ(bj::countAwareStrategy(h, R(6), r, false, false, false, 0.0),
              bj::basicStrategy(h, R(6), r, false, false, false));
}
