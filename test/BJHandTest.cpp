//
// Created by richard on 8/4/19.
//

#include "../src/BJHand.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"

GTEST("BJHand", "Creating new hand has bet value")
{
  BJHand testHand(5);
  EXPECT_EQ(5, testHand.getBet());
}

// random number of cards not including Ace test total
GTEST("BJHand", "Add random number of cards verify count and total")
{
  uint numCards = getRandomInt(0, 100);
  int expectedTotal = 0;
  BJHand testHand(0);

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

GTEST("BJHand", "Test Busted Hand")
{
  std::vector<int> cardValues{8, 12, 4};

  BJHand testHand(0);

  for(int cardValue : cardValues)
  {
    testHand.add(card(cardValue));
  }

  EXPECT_TRUE(testHand.isBusted());
}

GTEST("BJHand", "test for hand isSoft")
{
  card testAce(1);
  card testCard(7);

  BJHand testHand(0);

  testHand.add(testAce);
  testHand.add(testCard);

  EXPECT_EQ(18, testHand.getTotal());
  EXPECT_TRUE(testHand.isSoft());

  testHand.add(testCard);

  EXPECT_EQ(15, testHand.getTotal());
  EXPECT_FALSE(testHand.isSoft());
}

GTEST("BJHand", "test for blackjack is true")
{
  card testAce(1);
  card testTen(10);

  BJHand testHand(0);

  testHand.add(testAce);
  testHand.add(testTen);

  EXPECT_TRUE(testHand.isBlackJack());
}

GTEST("BJHand", "test isBlackJack is false")
{
  card testAce(1);
  card testFive(5);

  BJHand testHand(0);

  testHand.add(testAce);
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());

  testHand.clear();
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());
}