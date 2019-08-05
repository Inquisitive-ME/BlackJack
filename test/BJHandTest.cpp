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
  card testAce = card(1);
  card testFive = card(5);
  card testSeven = card(7);
  card testTen = card(10);

  BJHandTest(){
    // You can do set-up work for each test here.
  }

  ~BJHandTest() override{
    // You can do clean-up work that doesn't throw exceptions here.
  }



  void SetUp() override{
    // Code here will be called immediately after the constructor (right
    // before each test).
    BJHand testHand(5);
    testAce.flip();
    testFive.flip();
    testSeven.flip();
    testTen.flip();
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test suite.
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

GTEST("BJHand", "Test Busted Hand")
{
  std::vector<card> testCards{card(8), card(12), card(4)};

  BJHand testHand(0);

  for(card testCard : testCards)
  {
    testHand.add(testCard);
    testCard.flip();
  }

  EXPECT_TRUE(testHand.isBusted());
}

GTEST("BJHand", "test for hand isSoft")
{
  card testAce(1);
  testAce.flip();
  card testCard(7);
  testCard.flip();

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