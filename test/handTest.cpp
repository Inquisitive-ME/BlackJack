//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"
#include "../src/getRandomInt.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class MockCard: public card{
public:
  MockCard() : card(getRandomInt(1, 13)) {}
};

TEST(Hand, Add_random_number_of_cards_verify_count)
{
    uint numCards = (uint) getRandomInt(0, 100);
    hand testHand;

    for (uint i = 0; i < numCards; i++)
    {
      MockCard testCard;
      testHand.add(testCard);
    }
    ASSERT_EQ(numCards, testHand.getNumCards()) << "Add " + std::to_string(numCards) + " cards to hand" << std::endl;

    testHand.clear();
    ASSERT_EQ(uint(0), testHand.getNumCards());
}

TEST(Hand, Verify_Giving_card_from_one_hand_to_another)
{
    hand giveHand;
    hand receiveHand;
    uint emptyHandSize = 0;
    uint expectedReceiveHandSize = 1;

    MockCard testCard;

    giveHand.add(testCard);
    giveHand.give(0, receiveHand);

    EXPECT_EQ(emptyHandSize, giveHand.getNumCards());
    EXPECT_EQ(expectedReceiveHandSize, receiveHand.getNumCards());

    ASSERT_TRUE(testCard == receiveHand.getCard(0));
}

TEST(Hand, Verify_giving_card_from_non_zero_index_in_a_hand_to_another_hand)
{
  int indexToGive = getRandomInt(0, 9);
  card expectedCard(0);

  hand giveHand;
  hand receiveHand;

  for(int i = 0; i < 10; i++)
  {
    MockCard testCard;
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
  MockCard testCard;
  testHand.add(testCard);

  testHand.flipCard(0);
  ASSERT_TRUE(testHand.getCard(0).isFaceUp());
  testHand.flipCard(0);
  ASSERT_FALSE(testHand.getCard(0).isFaceUp());
}



