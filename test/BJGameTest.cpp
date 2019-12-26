//
// Created by richard on 12/23/19.
// Test for BJGame, including integration tests
//

#include "../src/BJGame.h"
#include "../src/deck.h"
#include <algorithm>

#include "gmock/gmock.h"
#include "GUnit/GTest.h"

//TODO Figure out how to update google test version
class MockDeck: public deck{
public:
    MOCK_METHOD2(deal, void(hand&, int));
    MOCK_METHOD2(deal, void(player&, int));
};

class MockDealer: public dealer{
public:
    MOCK_METHOD1(flipCard, void(int));
};

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
    deck startingDeck;
    startingDeck.populate();
    int initialNumCards = startingDeck.getNumCards();
    dealer testDealer;

    std::vector<abstractGamePlayer *> playerList = {&testPlayer};

    BJGameFunctions::starting_deal(startingDeck, testDealer, playerList);
    SHOULD("Every Player in game should have hand with correct bet and 2 cards")
    {
    std::for_each(playerList.begin(), playerList.end(), [this](abstractGamePlayer *resultPlayer){
        std::cout << "Check " << resultPlayer->getName() << std::endl;
        EXPECT_EQ(betForHand, resultPlayer->getHand(0).getBet());
        EXPECT_EQ((uint) 2, resultPlayer->getHand(0).getNumCards());
    });
    }
    SHOULD("Dealer should 2 cards with a total greater than 1")
    {
        EXPECT_EQ((uint) 2, testDealer.getNumCards());
        EXPECT_TRUE(testDealer.getTotal() > 0);
    }
    SHOULD("Deck lost 2 cards per player and 2 cards for the dealer")
    {
        EXPECT_EQ(2 + 2 * playerList.size(), initialNumCards - startingDeck.getNumCards());
    }
}

GTEST(BJGameTest, "Test evaluate_dealer_has_blackjack")
{
    using namespace testing;
    MockDeck testDeck;
    testDeck.populate();
    MockDealer testDealer;
    std::vector<abstractGamePlayer *> playerList = {&testPlayer};
    EXPECT_CALL(testDeck, deal(Matcher<player&>(_), 2)).Times(1);
    EXPECT_CALL(testDeck, deal(testDealer, 2)).Times(1);
    EXPECT_CALL(testDealer, flipCard(_));
    BJGameFunctions::starting_deal(testDeck, testDealer, playerList);

}
