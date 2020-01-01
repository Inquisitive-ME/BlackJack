//
// Created by richard on 8/3/19.
//


#include "../src/player.h"
#include "../src/getRandomInt.h"

#include "gtest/gtest.h"
#include "myGTest.h"
#include <string>

class playerTest : public::testing::Test{
public:
  int betForHand = 5;
  player testPlayer;
  card testAce = card(1);
  card testFive = card(5);
  card testTen = card(10);

  void SetUp() override{
    testPlayer.newHand(betForHand);
    testAce.flip();
    testFive.flip();
    testTen.flip();
  }

};

TEST(Player, Creating_player_has_name_and_purse)
{
  std::string testName = "test name";
  float testPurse = 10;

  player testPlayer(testName, testPurse);
  ASSERT_EQ(testName, testPlayer.getName());
  ASSERT_EQ(testPurse, testPlayer.getPurse());
}

TEST_F(playerTest, testing_new_hand_function)
{
    ASSERT_EQ(uint(1), testPlayer.numHands());
    ASSERT_EQ(betForHand, testPlayer.getHands()[0].getBet());
}

TEST_F(playerTest, testing_player_win_function)
{
  testPlayer.winHand(0);
  ASSERT_EQ(betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, player_wins_with_blackjack)
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testTen);

  testPlayer.winHand(0);
  ASSERT_EQ(betForHand * 1.5, testPlayer.getPurse());
}

TEST_F(playerTest, testing_player_win_with_total_21)
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testFive);
  testPlayer.getHand(0).add(testFive);

  testPlayer.winHand(0);
  ASSERT_EQ(betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, testing_player_lose_function)
{
  testPlayer.loseHand(0);
  ASSERT_EQ(-betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, test_player_double_down)
{
  testPlayer.doubleDown(0);
  ASSERT_EQ(betForHand * 2, testPlayer.getHand(0).getBet());
}

TEST_F(playerTest, test_dealer_bust_function_with_2_non_busted_hands)
{
  testPlayer.newHand(betForHand);
  testPlayer.dealerBusted();
  ASSERT_EQ(betForHand * 2, testPlayer.getPurse());
}

TEST_F(playerTest, test_dealer_bust_function_with_1_busted_hand)
{
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testFive);
  testPlayer.newHand(betForHand);
  testPlayer.newHand(betForHand);
  testPlayer.getHand(2).add(testTen);

  testPlayer.dealerBusted();
  float expectedPurse = betForHand + betForHand - betForHand;
  ASSERT_EQ(expectedPurse, testPlayer.getPurse());
}

TEST_F(playerTest, test_dealer_bust_function_with_1_blackjack_hand)
{
  testPlayer.getHand(0).add(testTen);
  testPlayer.getHand(0).add(testAce);
  testPlayer.newHand(betForHand);

  testPlayer.dealerBusted();
  float expectedPurse = betForHand * 1.5 + betForHand;
  ASSERT_EQ(expectedPurse, testPlayer.getPurse());
}

TEST_F(playerTest, test_player_surrender)
{
  testPlayer.surrender(0);
  EXPECT_EQ(-betForHand * 0.5, testPlayer.getPurse());
}

TEST_F(playerTest, test_player_split)
{
  testPlayer.getHand(0).add(testAce);
  testPlayer.getHand(0).add(testFive);

  testPlayer.split(0);

  GTEST_LOG << "Player has 2 hands" << std::endl;
    ASSERT_EQ(uint(2), testPlayer.getHands().size());

  GTEST_LOG << "Each hand should have the starting bet value" << std::endl;
    EXPECT_EQ(betForHand, testPlayer.getHand(0).getBet());
    EXPECT_EQ(betForHand, testPlayer.getHand(1).getBet());

  GTEST_LOG << "First hand should have first card of original hand, second hand should have second card" << std::endl;
    EXPECT_EQ(uint(1), testPlayer.getHand(0).getNumCards());
    ASSERT_TRUE(testAce == testPlayer.getHand(0).getCard(0));
    EXPECT_EQ(uint(1), testPlayer.getHand(1).getNumCards());
    ASSERT_TRUE(testFive == testPlayer.getHand(1).getCard(0));
}