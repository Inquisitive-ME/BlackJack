//
// Created by richard on 8/4/19.
//


#include "../src/deck.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"

//class playerTest : public::testing::Test{
//public:
//  int betForHand = 5;
//  player testPlayer;
//  card testAce = card(1);
//  card testFive = card(5);
//  card testTen = card(10);
//
//  void SetUp() override{
//    testPlayer.newHand(betForHand);
//  }
//
//};

GTEST("Deck", "Test populate method")
{
  int numDecks = 6;
  deck testDeck(numDecks, 0);

  testDeck.populate();

  SHOULD("Number of cards should be number of decks * 52")
  {
    EXPECT_EQ(uint(numDecks * 52), testDeck.getNumCards());
  }
}