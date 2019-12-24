//
// Created by richard on 12/23/19.
// Test for BJGame, including integration tests
//

#include "../src/BJGame.h"
#include <algorithm>

#include "GUnit/GTest.h"

class TestImplementationPlayer: public abstractGamePlayer{
public:
    TestImplementationPlayer(std::string playerName) : abstractGamePlayer(playerName){};
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

    BJGameTest() : testPlayer("player1"), testAce(1), testFive(5), testSeven(7), testTen(10)
    {
        testAce.flip();
        testFive.flip();
        testSeven.flip();
        testTen.flip();
        testPlayer.setBet(betForHand);
    }
};

GTEST(BJGameTest, "Test starting deal")
{
    BJGame testGame;
    deck startingDeck = testGame.getDeck();

    std::vector<abstractGamePlayer *> playerList = {&testPlayer};
    testGame.startGame(playerList);
    testGame.starting_deal();
    testGame.printDeck();
    SHOULD("Every Player in game should have hand with correct bet and 2 cards")
    {
    std::for_each(playerList.begin(), playerList.end(), [this](abstractGamePlayer *resultPlayer){
        std::cout << "Check " << resultPlayer->getName() << std::endl;
        EXPECT_EQ(betForHand, resultPlayer->getHand(0).getBet());
        EXPECT_EQ(2, resultPlayer->getHand(0).getNumCards());
    });
    }
    SHOULD("Dealer should 2 cards with a total greater than 1")
    {
        EXPECT_EQ(2, testGame.getDealer().getNumCards());
        EXPECT_TRUE(testGame.getDealer().getTotal() > 0);
    }
    SHOULD("Deck lost 2 cards per player and 2 cards for the dealer")
    {
        EXPECT_EQ(2 + 2 * playerList.size(), startingDeck.getNumCards() - testGame.getDeck().getNumCards());
    }
}