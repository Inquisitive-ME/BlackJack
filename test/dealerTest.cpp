//
// Created by richard on 8/4/19.
//

#include "../src/dealer.h"

#include "GUnit/GTest.h"

class dealerTest : public::testing::Test{
public:
  dealer testDealer;
  card testAce = card(1);
  card testFive = card(5);
  card testTen = card(10);

  void SetUp() override{
    testAce.flip();
    testFive.flip();
    testTen.flip();
  }

};
GTEST(dealerTest, "test is hitting not soft")
{
  testDealer.add(testFive);
  testDealer.add(testTen);
  testDealer.add(testAce);

  EXPECT_TRUE(testDealer.isHitting());
}

GTEST(dealerTest, "test not hitting not soft")
{
  testDealer.add(testFive);
  testDealer.add(testTen);
  testDealer.add(testAce);
  testDealer.add(testAce);

  EXPECT_FALSE(testDealer.isHitting());
}

GTEST(dealerTest, "test hitting soft")
{
  testDealer.add(testFive);
  testDealer.add(testAce);
  testDealer.add(testAce);

  EXPECT_TRUE(testDealer.isHitting());
}

GTEST(dealerTest, "test not hitting soft")
{
  testDealer.add(testFive);
  testDealer.add(testAce);
  testDealer.add(testAce);
  testDealer.add(testAce);

  EXPECT_FALSE(testDealer.isHitting());
}