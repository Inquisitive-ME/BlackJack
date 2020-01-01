//
// Created by richard on 8/4/19.
//

#include "../src/BJHand.h"
#include "../src/getRandomInt.h"

#include "gtest/gtest.h"
#include "myGTest.h"

class BJHandTest : public::testing::Test{
public:
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

TEST_F(BJHandTest, Creating_new_hand_has_bet_value)
{
  ASSERT_EQ(betForHand, testHand.getBet());
}

// random number of cards not including Ace test total
TEST(BJHand, Add_random_number_of_cards_verify_count_and_total)
{
    uint numCards = getRandomInt(0, 100);
    int expectedTotal = 0;
    BJHand testHand(0);

    GTEST_LOG << "Add " + std::to_string(numCards) + " cards to hand" << std::endl;

    for (uint i = 0; i < numCards; i++)
    {
      card testCard(getRandomInt(2, 13));
      testCard.flip();
      expectedTotal += testCard.getValue();
      testHand.add(testCard);
    }
    EXPECT_EQ(numCards, testHand.getNumCards());
    ASSERT_EQ(expectedTotal, testHand.getTotal());

    GTEST_LOG << "Clear Hand" << std::endl;
    testHand.clear();
    EXPECT_EQ(uint(0), testHand.getNumCards());
    ASSERT_EQ(0, testHand.getTotal());
}

TEST_F(BJHandTest, Busted_hand_is_busted)
{
    testHand.add(testTen);
    testHand.add(testFive);
    testHand.add(testSeven);

    ASSERT_TRUE(testHand.isBusted());
}

TEST_F(BJHandTest, Hand_with_ace_isSoft)
{
    testHand.add(testAce);
    testHand.add(testSeven);

    EXPECT_EQ(18, testHand.getTotal());
    ASSERT_TRUE(testHand.isSoft());

    testHand.add(testSeven);

    EXPECT_EQ(15, testHand.getTotal());
    ASSERT_FALSE(testHand.isSoft());
}

TEST_F(BJHandTest, Blackjack_is_true)
{
    testHand.add(testAce);
    testHand.add(testTen);

    ASSERT_TRUE(testHand.isBlackJack());
}

TEST_F(BJHandTest, BlackJack_is_false_with_ace_as_1)
{
    testHand.add(testAce);
    testHand.add(testFive);
    testHand.add(testFive);

    ASSERT_FALSE(testHand.isBlackJack());
}

TEST_F(BJHandTest, BlackJack_is_false)
{
    testHand.add(testFive);
    testHand.add(testFive);

    ASSERT_FALSE(testHand.isBlackJack());
}