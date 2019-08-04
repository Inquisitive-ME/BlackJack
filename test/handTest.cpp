//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"
#include <string>

// TODO try mocking to mock cards

GTEST("Hand", "Creating new hand has bet value")
{
  hand testHand(5);
  EXPECT_EQ(5, testHand.getBet());

}

// random number of cards not including Ace test total
GTEST("Hand", "Add random number of cards verify count and total")
{
  uint numCards = getRandomInt(0, 100);
  int expectedTotal = 0;
  hand testHand(0);

  SHOULD("Add " + std::to_string(numCards) + " cards to hand")
  {
    for (uint i = 0; i < numCards; i++)
    {
      card testCard(getRandomInt(2, 13));
      expectedTotal += testCard.getValue();
      testHand.add(testCard);
    }
    EXPECT_EQ(numCards, testHand.getNumCards());
    EXPECT_EQ(expectedTotal, testHand.getTotal());
  }

  SHOULD("Clear Hand")
  {
    testHand.clear();
    EXPECT_EQ(uint(0), testHand.getNumCards());
    EXPECT_EQ(0, testHand.getTotal());
  }
}

// Give to another hand
GTEST("Hand", "Verify Giving card from one hand to another")
{
  hand giveHand(0);
  hand receiveHand(0);
  uint expectedEndGiveHandSize = 0;

  card testCard(getRandomInt(1, 13));

  giveHand.add(testCard);
  giveHand.give(0, receiveHand);

  SHOULD("receiving hand has same card as given to it")
  {
    EXPECT_EQ(testCard.print(), receiveHand.getCards()[0].print());
  }

  SHOULD("hand giving card should have no cards and a total of 0")
  {
    EXPECT_EQ(expectedEndGiveHandSize, giveHand.getNumCards());
    EXPECT_EQ(0, giveHand.getTotal());
  }
}

GTEST("Hand", "Verify giving card from non zero index in a hand to another hand")
{
  int indexToGive = getRandomInt(0, 9);
  std::string expectedResult = "";

  hand giveHand(0);
  hand receiveHand(0);

  for(int i = 0; i < 10; i++)
  {
    card testCard(getRandomInt(1,13));
    giveHand.add(testCard);
    if(i == indexToGive)
    {
      expectedResult = testCard.print();
    }
  }

  giveHand.give(indexToGive, receiveHand);
  EXPECT_EQ(expectedResult, receiveHand.getCards()[0].print());
}

// busted
GTEST("Hand", "Test Busted Hand")
{
  std::vector<int> cardValues{8, 12, 4};

  hand testHand(0);

  for(int cardValue : cardValues)
  {
    testHand.add(card(cardValue));
  }

  EXPECT_TRUE(testHand.isBusted());

}

// soft
GTEST("Hand", "test for hand isSoft")
{
  card testAce(1);
  card testCard(7);

  hand testHand(0);

  testHand.add(testAce);
  testHand.add(testCard);

  EXPECT_EQ(18, testHand.getTotal());
  EXPECT_TRUE(testHand.isSoft());

  testHand.add(testCard);

  EXPECT_EQ(15, testHand.getTotal());
  EXPECT_FALSE(testHand.isSoft());
}

GTEST("Hand", "test for blackjack is true")
{
  card testAce(1);
  card testTen(10);

  hand testHand(0);

  testHand.add(testAce);
  testHand.add(testTen);

  EXPECT_TRUE(testHand.isBlackJack());
}

GTEST("Hand", "test isBlackJack is false")
{
  card testAce(1);
  card testFive(5);

  hand testHand(0);

  testHand.add(testAce);
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());

  testHand.clear();
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());
}

