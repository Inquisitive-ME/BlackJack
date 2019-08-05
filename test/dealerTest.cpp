//
// Created by richard on 8/4/19.
//

#include "../src/dealer.h"

#include "GUnit/GTest.h"

GTEST("Dealer", "test is hitting")
{
  dealer testDealer;
  card testCard(5);

  testDealer.add(testCard);

  EXPECT_TRUE(testDealer.isHitting());
}
