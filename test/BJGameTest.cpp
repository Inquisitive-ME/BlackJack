//
// Created by richard on 12/23/19.
// Test for BJGame, including integration tests
//

#include "../src/bj_game.h"
#include "../src/deck.h"
#include "../src/dealer_impl.h"
#include <algorithm>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class MockDeck : public deck {
public:
    MOCK_METHOD2(dealFaceDown, void(hand&, int));
    MOCK_METHOD2(dealFaceUp, void(PlayerInterface&, int));
};

class MockDealer : public DealerInterface {
public:
    MOCK_CONST_METHOD0(isHitting, const bool());
    MOCK_METHOD0(getFaceUpCards, std::vector<card>());

    MOCK_METHOD1(flipCard, void(int));
    MOCK_METHOD1(getCard, const card(int));
};

class MockPlayer : public PlayerInterface {
public:
    MOCK_METHOD1(winHand, void(int));
    MOCK_METHOD1(loseHand, void(int));
    MOCK_METHOD1(pushHand, void(int));

    MOCK_METHOD0(dealerBusted, void());
    MOCK_METHOD1(split, void(int));
    MOCK_METHOD1(doubleDown, void(int));
    MOCK_METHOD1(surrender, void(int));
    MOCK_CONST_METHOD0(numHands, const uint());
    MOCK_METHOD1(getHand, BJHand &(uint));
    MOCK_METHOD0(getHands, std::vector<BJHand> & ());
    MOCK_METHOD1(removeHand, void(int));
    MOCK_METHOD0(clearAllHands, void());
    MOCK_CONST_METHOD0(getPurse, const float());
    MOCK_CONST_METHOD0(getName, const std::string());
    MOCK_METHOD0(getBet, int());
    MOCK_METHOD1(setBet, void(int bet));
    MOCK_METHOD1(newHand, void(float));
    MOCK_METHOD0(payInsurance, void());
};

class MockAI : public AiInterface {
public:
    MOCK_METHOD1(getPlayerBet, const int(const PlayerInterface&));
    MOCK_METHOD2(getMove, const MOVES(std::vector<card>, const BJHand &));
    MOCK_METHOD0(continuePlaying,const bool());
    MOCK_METHOD1(payInsurance, const bool(const PlayerInterface &));
    MOCK_METHOD0(illegalMove, bool());

};

TEST(BJGameFunctions, deal_to_all_players) {
    using namespace std;
    using namespace testing;
    MockDeck testDeck;
    MockPlayer testPlayer1;
    MockPlayer testPlayer2;
    MockAI testAI;

    vector<PlayerInterface *> playerList = {&testPlayer1, &testPlayer2};

    EXPECT_CALL(testAI, getPlayerBet(Matcher<const PlayerInterface &>(Eq(ByRef(testPlayer1))))).WillOnce(Return(1));
    EXPECT_CALL(testPlayer1, newHand(1));
    EXPECT_CALL(testDeck, dealFaceUp(Matcher<PlayerInterface &>(Eq(ByRef(testPlayer1))), 2));

    EXPECT_CALL(testAI, getPlayerBet(Matcher<const PlayerInterface &>(Eq(ByRef(testPlayer2))))).WillOnce(Return(2));
    EXPECT_CALL(testPlayer2, newHand(2));
    EXPECT_CALL(testDeck, dealFaceUp(Matcher<PlayerInterface &>(Eq(ByRef(testPlayer2))), 2));

    BJGameFunctions::deal_to_all_players(testDeck, playerList, testAI);
}

TEST(BJGameFunctions, deal_to_dealer) {
    using namespace std;
    using namespace testing;
    MockDeck testDeck;
    MockDealer testDealer;

    EXPECT_CALL(testDeck, dealFaceDown(testDealer, 2));
    EXPECT_CALL(testDealer, flipCard(_));

    BJGameFunctions::deal_to_dealer(testDeck, testDealer);
}

TEST(BJGameFunctions, next_state_deal_dealer_has_ace_faceup) {
    using namespace std;
    using namespace testing;
    DealerImpl testDealer;
    testDealer.copy_to_hand(card(1));

    GameState result = BJGameFunctions::deal_next_state(testDealer);

    EXPECT_EQ(result, INSURANCE);
}

TEST(BJGameFunctions, next_state_deal_dealer_no_ace_faceup) {
    using namespace std;
    using namespace testing;
    DealerImpl testDealer;
    testDealer.copy_to_hand(card(2));

    GameState result = BJGameFunctions::deal_next_state(testDealer);

    EXPECT_EQ(result, MOVE);
}