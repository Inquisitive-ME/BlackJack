//
// Created by richard on 8/3/19.
//


#include "../src/player.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"
#include <string>

//TODO use mocks to mock the hand class

GTEST("Player", "Creating player has name and purse")
{
  std::string testName = "test name";
  float testPurse = 10;

  player testPlayer(testName, testPurse);
  EXPECT_EQ(testName, testPlayer.getName());
  EXPECT_EQ(testPurse, testPlayer.getPurse());

}

GTEST("Player", "testing new hand function")
{
  int betForHand = 5;
  player testPlayer;
  testPlayer.newHand(betForHand);

  SHOULD("Added new hand to player hands")
  {
    EXPECT_EQ(uint(1), testPlayer.numHands());
  }
  SHOULD("new hand should have correct bet")
  {
     EXPECT_EQ(betForHand, testPlayer.getHands()[0].getBet());
  }
}

GTEST("Player", "testing player win function")
{
  player testPlayer;
  int betForHand = 5;
  testPlayer.newHand(betForHand);

  testPlayer.win();
  EXPECT_EQ(betForHand, testPlayer.getPurse());
}

GTEST("Player", "player wins with blackjack")
{
  player testPlayer;
  int betForHand = 5;
  testPlayer.newHand(betForHand);

  testPlayer.win();
  EXPECT_EQ(betForHand, testPlayer.getPurse());

}

GTEST("Player", "testing player win function")
{
  player testPlayer;
  int betForHand = 5;
  testPlayer.newHand(betForHand);

  testPlayer.win();
  EXPECT_EQ(betForHand, testPlayer.getPurse());
}

//blackjack

//doubledown

//surrender

//busted

//split