//
// Created by richard on 8/4/19.
//

#include "../src/BJHand.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"

class BJHandTest : public::testing::Test{
public:
  //TODO need to clean this up so it works
  int betForHand = 5;
  BJHand testHand;
  card testAce;
  card testFive;
  card testSeven;
  card testTen;

  BJHandTest() : testHand(betForHand), testAce(1), testFive(5), testSeven(7), testTen(10)
  {
    testAce.flip();
    testFive.flip();
    testSeven.flip();
    testTen.flip();
  }
};

GTEST(BJHandTest, "Creating new hand has bet value")
{
  EXPECT_EQ(betForHand, testHand.getBet());
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
      testCard.flip();
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

GTEST(BJHandTest, "Test Busted Hand")
{
  testHand.add(testTen);
  testHand.add(testFive);
  testHand.add(testSeven);

  EXPECT_TRUE(testHand.isBusted());
}

GTEST(BJHandTest, "test for hand isSoft")
{

  testHand.add(testAce);
  testHand.add(testSeven);

  EXPECT_EQ(18, testHand.getTotal());
  EXPECT_TRUE(testHand.isSoft());

  testHand.add(testSeven);

  EXPECT_EQ(15, testHand.getTotal());
  EXPECT_FALSE(testHand.isSoft());
}

GTEST(BJHandTest, "test for blackjack is true")
{
  testHand.add(testAce);
  testHand.add(testTen);

  EXPECT_TRUE(testHand.isBlackJack());
}

GTEST(BJHandTest, "test isBlackJack is false")
{
  testHand.add(testAce);
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());

  testHand.clear();
  testHand.add(testFive);
  testHand.add(testFive);

  EXPECT_FALSE(testHand.isBlackJack());
}