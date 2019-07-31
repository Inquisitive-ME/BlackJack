//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"
#include <string>

//TODO get Total Feature test to do different combinations


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

// busted

// soft

// total with ace as 1

// print
