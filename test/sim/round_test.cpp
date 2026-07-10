#include "sim/round.h"
#include "sim/cards.h"
#include "sim/hand.h"
#include "sim/rules.h"

#include "gtest/gtest.h"

#include <vector>

using bj::Action;
using bj::Card;
using bj::Hand;
using bj::RulesConfig;

namespace {

// A deterministic shoe that deals a prescribed sequence -- lets us build exact
// blackjack scenarios (the real Shoe is shuffled). Same interface playRound uses.
struct StackedShoe {
    std::vector<Card> cards;
    std::size_t pos = 0;
    Card dealFaceUp() { return cards[pos++]; }
    Card dealHole() { return cards[pos++]; }
    void reveal(Card) {}
};

// Rank r (2..9) -> bucket r-1; ACE=0; ten group=9.
constexpr Card R(int rank) {
    return rank == 1 ? Card(0) : rank <= 9 ? Card(rank - 1) : Card(9);
}

// Policies with the playRound signature (hand, up, rules, canDouble, canSplit, canSurrender).
Action alwaysStand(const Hand &, Card, const RulesConfig &, bool, bool, bool) { return Action::Stand; }
Action alwaysDouble(const Hand &, Card, const RulesConfig &, bool cd, bool, bool) {
    return cd ? Action::Double : Action::Stand;
}
Action alwaysSurrender(const Hand &, Card, const RulesConfig &, bool, bool, bool cs) {
    return cs ? Action::Surrender : Action::Stand;
}
Action splitThenStand(const Hand &, Card, const RulesConfig &, bool, bool canSplit, bool) {
    return canSplit ? Action::Split : Action::Stand;
}

} // namespace

TEST(SettleHand, BasicOutcomes) {
    Hand p20, p18, dealer19, busted;
    p20.add(bj::TEN); p20.add(bj::TEN);
    p18.add(bj::TEN); p18.add(R(8));
    dealer19.add(bj::TEN); dealer19.add(R(9));
    busted.add(bj::TEN); busted.add(bj::TEN); busted.add(R(5));

    EXPECT_EQ(bj::settleHand(p20, dealer19, 1.0), +1.0); // win
    EXPECT_EQ(bj::settleHand(p18, dealer19, 1.0), -1.0); // lose
    EXPECT_EQ(bj::settleHand(dealer19, dealer19, 1.0), 0.0); // push (equal totals)
    EXPECT_EQ(bj::settleHand(busted, dealer19, 1.0), -1.0); // player bust loses
    EXPECT_EQ(bj::settleHand(p20, busted, 2.0), +2.0); // dealer bust, doubled bet
    EXPECT_EQ(bj::settleHand(busted, busted, 1.0), -1.0); // both bust: player still loses
}

TEST(Round, NaturalPaysThreeToTwo) {
    RulesConfig rules;
    StackedShoe shoe{{bj::ACE, R(7), bj::TEN, R(5)}}; // player A,10 ; dealer 7 up
    EXPECT_EQ(bj::playRound(shoe, rules, alwaysStand), 1.5);
}

TEST(Round, BothNaturalsPush) {
    RulesConfig rules;
    StackedShoe shoe{{bj::ACE, bj::ACE, bj::TEN, bj::TEN}}; // both blackjack, dealer Ace up (peeks)
    EXPECT_EQ(bj::playRound(shoe, rules, alwaysStand), 0.0);
}

TEST(Round, DealerNaturalPlayerLoses) {
    RulesConfig rules;
    StackedShoe shoe{{R(9), bj::ACE, R(8), bj::TEN}}; // player 17 ; dealer A + 10 = BJ
    EXPECT_EQ(bj::playRound(shoe, rules, alwaysStand), -1.0);
}

TEST(Round, SurrenderIsTerminalAndSettledOnce) {
    RulesConfig rules;
    rules.surrenderAllowed = true;
    // player 16 (10,6) vs dealer 10 up, hole 5 (dealer 15, no BJ).
    StackedShoe shoe{{bj::TEN, bj::TEN, R(6), R(5)}};
    // Exactly -0.5, NOT settled a second time against the dealer.
    EXPECT_EQ(bj::playRound(shoe, rules, alwaysSurrender), -0.5);
}

TEST(Round, DoubleTakesExactlyOneCardThenStops) {
    RulesConfig rules;
    // player 11 (5,6) ; dealer 9 up, hole 7 (=16) then draws a 10 and busts.
    // The doubled card is a 10 -> player 21. Reward is +2 (doubled bet, dealer bust).
    StackedShoe shoe{{R(5), R(9), R(6), R(7), bj::TEN, bj::TEN}};
    EXPECT_EQ(bj::playRound(shoe, rules, alwaysDouble), +2.0);
}

TEST(Round, SplitDealsASecondCardToEachHand) {
    RulesConfig rules;
    // player 8,8 ; dealer 9 up + 9 hole = 18 (stands, no draw).
    // Each split hand receives a 10 -> 18, so both push (0) IF the second cards
    // are dealt. If split dealt no second card (the legacy bug), each hand would
    // be a lone 8 and lose -> -2. Distinguishes the fix.
    StackedShoe shoe{{R(8), R(9), R(8), R(9), bj::TEN, bj::TEN}};
    EXPECT_EQ(bj::playRound(shoe, rules, splitThenStand), 0.0);
}

TEST(Round, SplitAcesGetOneCardEach) {
    RulesConfig rules;
    // A,A split -> each ace gets one card (10) = soft 21, then stands (no more cards).
    // Dealer 9 up + 9 hole = 18 stands. Both 21 beat 18 -> +2.
    StackedShoe shoe{{bj::ACE, R(9), bj::ACE, R(9), bj::TEN, bj::TEN}};
    EXPECT_EQ(bj::playRound(shoe, rules, splitThenStand), +2.0);
}
