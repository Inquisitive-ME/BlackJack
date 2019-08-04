//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"

GTEST("Hand", "Add random number of cards verify count and total")
{
  uint numCards = getRandomInt(0, 100);
  hand testHand;

  SHOULD("Add " + std::to_string(numCards) + " cards to hand")
  {
    for (uint i = 0; i < numCards; i++)
    {
      card testCard(getRandomInt(1, 13));
      testHand.add(testCard);
    }
    EXPECT_EQ(numCards, testHand.getNumCards());
  }

  SHOULD("Clear Hand")
  {
    testHand.clear();
    EXPECT_EQ(uint(0), testHand.getNumCards());
  }
}

// Give to another hand
GTEST("Hand", "Verify Giving card from one hand to another")
{
  hand giveHand;
  hand receiveHand;
  uint expectedEndGiveHandSize = 0;

  card testCard(getRandomInt(1, 13));

  giveHand.add(testCard);
  giveHand.give(0, receiveHand);

  SHOULD("receiving hand has same card as given to it")
  {
    EXPECT_TRUE(testCard == receiveHand.getCard(0));
  }

  SHOULD("hand giving card should have no cards")
  {
    EXPECT_EQ(expectedEndGiveHandSize, giveHand.getNumCards());
  }
}

GTEST("Hand", "Verify giving card from non zero index in a hand to another hand")
{
  int indexToGive = getRandomInt(0, 9);
  card expectedCard(0);

  hand giveHand;
  hand receiveHand;

  for(int i = 0; i < 10; i++)
  {
    card testCard(getRandomInt(1,13));
    giveHand.add(testCard);
    if(i == indexToGive)
    {
      expectedCard = testCard;
    }
  }

  giveHand.give(indexToGive, receiveHand);
  EXPECT_TRUE(expectedCard == receiveHand.getCard(0));
}



