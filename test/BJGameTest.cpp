//
// Created by richard on 12/23/19.
// Test for BJGame, including integration tests
//

#include "../src/BJGame.h"

#include "GUnit/GTest.h"

class TestImplementationPlayer: public abstractGamePlayer{
public:
    int getBet(){return betForHand;};
    //virtual bool purchaseInsurance(int count);
    MOVES getMove(){return HIT;};
    void setBet(int bet)
    {
        betForHand = bet;
    }

private:
    int betForHand;

};

class BJGameTest : public::testing::Test{
public:
    //TODO need to clean this up so it works
    int betForHand = 5;
    TestImplementationPlayer testPlayer;
    card testAce;
    card testFive;
    card testSeven;
    card testTen;

    BJGameTest() : testPlayer(), testAce(1), testFive(5), testSeven(7), testTen(10)
    {
        testAce.flip();
        testFive.flip();
        testSeven.flip();
        testTen.flip();
        testPlayer.setBet(betForHand);
    }
};

GTEST(BJGameTest, "Starting Deal")
{
    BJGame testGame;

    std::vector<abstractGamePlayer *> tempPlayerList = {&testPlayer};
    testGame.startGame(tempPlayerList);
    testGame.starting_deal();
    std::vector<abstractGamePlayer *> playerList = testGame.getPlayers();
    abstractGamePlayer* resultPlayer = playerList[0];

    EXPECT_EQ(betForHand, resultPlayer->getHand(0).getBet());
}