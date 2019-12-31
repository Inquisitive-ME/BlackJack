//
// Created by richard on 12/23/19.
// Test for BJGame, including integration tests
//

#include "../src/BJGame.h"
#include "../src/deck.h"
#include <algorithm>

#include "gmock/gmock.h"
#include "GUnit/GTest.h"

class MockDeck: public deck{
public:
    MOCK_METHOD2(deal, void(hand&, int));
    MOCK_METHOD2(deal, void(player&, int));
};

class MockDealer: public dealer{
public:
    MOCK_METHOD1(flipCard, void(int));
};

class MockPlayer: public abstractGamePlayer{
public:
    MOCK_METHOD0(getBet, int());
    MOCK_METHOD0(getMove, MOVES());
    MOCK_METHOD1(setBet, void(int bet));
    MOCK_METHOD1(newHand, void(float));
};

TEST(BJGameFunctions, test_starting_deal)
{
    using namespace std;
    using namespace testing;
    MockDeck testDeck;
    MockDealer testDealer;
    MockPlayer testPlayer1;
    MockPlayer testPlayer2;

    vector<abstractGamePlayer *> playerList = {&testPlayer1, &testPlayer2};

    EXPECT_CALL(testPlayer1, getBet()).WillOnce(Return(1));
    EXPECT_CALL(testPlayer1, newHand(1));
    EXPECT_CALL(testDeck, deal(Matcher<player&>(Eq(ByRef(testPlayer1))), 2));

    EXPECT_CALL(testPlayer2, getBet()).WillOnce(Return(2));
    EXPECT_CALL(testPlayer2, newHand(2));
    EXPECT_CALL(testDeck, deal(Matcher<player&>(Eq(ByRef(testPlayer2))), 2));

    EXPECT_CALL(testDeck, deal(testDealer, 2));
    EXPECT_CALL(testDealer, flipCard(_));

    BJGameFunctions::starting_deal(testDeck, testDealer, playerList);
}