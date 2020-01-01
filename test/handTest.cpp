//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"
#include "../src/getRandomInt.h"

#include "gtest/gtest.h"
#include "myGTest.h"

//TODO Should be mocking card for unit tests and only use card for integration test
TEST(Hand, Add_random_number_of_cards_verify_count)
{
    uint numCards = getRandomInt(0, 100);
    hand testHand;

    GTEST_LOG << "Add " + std::to_string(numCards) + " cards to hand" << std::endl;
    for (uint i = 0; i < numCards; i++)
    {
      card testCard(getRandomInt(1, 13));
      testHand.add(testCard);
    }
    ASSERT_EQ(numCards, testHand.getNumCards());

    GTEST_LOG << "Clear Hand" << std::endl;
    testHand.clear();
    ASSERT_EQ(uint(0), testHand.getNumCards());
}

TEST(Hand, Verify_Giving_card_from_one_hand_to_another)
{
    hand giveHand;
    hand receiveHand;
    uint expectedEndGiveHandSize = 0;

    card testCard(getRandomInt(1, 13));

    giveHand.add(testCard);
    giveHand.give(0, receiveHand);

    ASSERT_TRUE(testCard == receiveHand.getCard(0));

    ASSERT_EQ(expectedEndGiveHandSize, giveHand.getNumCards());
}

TEST(Hand, Verify_giving_card_from_non_zero_index_in_a_hand_to_another_hand)
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
  ASSERT_TRUE(expectedCard == receiveHand.getCard(0));
}

TEST(Hand, Verify_flip_card)
{
  hand testHand;
  card testCard(0);

  testHand.add(testCard);

  testHand.flipCard(0);
  ASSERT_TRUE(testHand.getCard(0).isFaceUp());
  testHand.flipCard(0);
  ASSERT_FALSE(testHand.getCard(0).isFaceUp());
}



