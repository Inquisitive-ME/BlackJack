//
// Created by richard on 8/3/19.
//


#include "../src/player.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"
#include <string>

class playerTest : public::testing::Test{
public:
  // TODO need to understand this better, does each test share the same player, and is the new hand called for each test?
  int betForHand = 5;
  player testPlayer;
  card testAce = card(1);
  card testFive = card(5);
  card testTen = card(10);

  void SetUp() override{
    testPlayer.newHand(betForHand);
  }

};

GTEST("Player", "Creating player has name and purse")
{
  std::string testName = "test name";
  float testPurse = 10;

  player testPlayer(testName, testPurse);
  EXPECT_EQ(testName, testPlayer.getName());
  EXPECT_EQ(testPurse, testPlayer.getPurse());

}

GTEST(playerTest, "testing new hand function")
{
  SHOULD("Added new hand to player hands")
  {
    EXPECT_EQ(uint(1), testPlayer.numHands());
  }
  SHOULD("new hand should have correct bet")
  {
     EXPECT_EQ(betForHand, testPlayer.getHands()[0].getBet());
  }
}

GTEST(playerTest, "testing player win function")
{
  testPlayer.winHand(0);
  EXPECT_EQ(betForHand, testPlayer.getPurse());
}

GTEST(playerTest, "player wins with blackjack")
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testTen);

  testPlayer.winHand(0);
  EXPECT_EQ(betForHand * 1.5, testPlayer.getPurse());

}

GTEST(playerTest, "testing player win with total = 21")
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testFive);
  testPlayer.getHand(0).add(testFive);

  testPlayer.winHand(0);
  EXPECT_EQ(betForHand, testPlayer.getPurse());
}

GTEST(playerTest, "testing player lose function")
{
  testPlayer.loseHand(0);
  EXPECT_EQ(-betForHand, testPlayer.getPurse());
}

GTEST(playerTest, "test player double down")
{
  testPlayer.doubleDown(0);
  EXPECT_EQ(betForHand * 2, testPlayer.getHand(0).getBet());

}

GTEST(playerTest, "test dealer bust function with 2 non busted hands")
{
  testPlayer.newHand(betForHand);
  testPlayer.dealerBusted();
  EXPECT_EQ(betForHand * 2, testPlayer.getPurse());

}

GTEST(playerTest, "test dealer bust function with 1 busted hand")
{
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testFive);
  testPlayer.newHand(betForHand);
  testPlayer.newHand(betForHand);
  testPlayer.getHand(2).add(testTen);

  testPlayer.dealerBusted();
  float expectedPurse = betForHand + betForHand - betForHand;
  EXPECT_EQ(expectedPurse, testPlayer.getPurse());

}

GTEST(playerTest, "test dealer bust function with 1 blackjack hand")
{
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testAce);
  testPlayer.newHand(betForHand);

  testPlayer.dealerBusted();
  float expectedPurse = betForHand * 1.5 + betForHand;
  EXPECT_EQ(expectedPurse, testPlayer.getPurse());

}

//surrender
GTEST(playerTest, "test player surrender")
{
  testPlayer.surrender(0);
  EXPECT_EQ(-betForHand * 0.5, testPlayer.getPurse());
}

//split
GTEST(playerTest, "test player split")
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testFive);

  testPlayer.split(0);

  SHOULD("Player has 2 hands")
  {
    EXPECT_EQ(uint(2), testPlayer.getHands().size());
  }
  SHOULD("Each hand should have the starting bet value")
  {
    EXPECT_EQ(betForHand, testPlayer.getHand(0).getBet());
    EXPECT_EQ(betForHand, testPlayer.getHand(1).getBet());
  }
  SHOULD("First hand should have first card of original hand, second hand should have second card")
  {
    EXPECT_EQ(uint(1), testPlayer.getHand(0).getNumCards());
    EXPECT_TRUE(testAce == testPlayer.getHand(0).getCard(0));
    EXPECT_EQ(uint(1), testPlayer.getHand(1).getNumCards());
    EXPECT_TRUE(testFive == testPlayer.getHand(1).getCard(0));
  }

}