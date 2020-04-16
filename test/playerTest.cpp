//
// Created by richard on 8/3/19.
//


#include "../src/player_impl.h"
#include "../src/get_random_int.h"

#include "gtest/gtest.h"
#include <string>
#include <gmock/gmock-matchers.h>

class playerTest : public ::testing::Test {
public:
    int betForHand = 5;
    PlayerImpl testPlayer;
    card testAce = card(1);
    card testFive = card(5);
    card testTen = card(10);

    void SetUp() override {
        testPlayer.newHand(betForHand);
        testAce.flip();
        testFive.flip();
        testTen.flip();
    }

};

TEST(Player, Creating_player_has_name_and_purse) {
    std::string testName = "test name";
    float testPurse = 10;

    PlayerImpl testPlayer(testName, testPurse);
    ASSERT_EQ(testName, testPlayer.getName());
    ASSERT_EQ(testPurse, testPlayer.getPurse());
}

TEST_F(playerTest, Win_function) {
    testPlayer.winHand(0);
    ASSERT_EQ(betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, Win_with_blackjack) {
    testPlayer.moveToHand(0, testAce);
    testPlayer.moveToHand(0, testTen);

    testPlayer.winHand(0);
    ASSERT_EQ(betForHand * 1.5, testPlayer.getPurse());
}

TEST_F(playerTest, Win_with_total_21_not_blackjack) {
    testPlayer.moveToHand(0, testAce);
    testPlayer.moveToHand(0, testFive);
    testPlayer.moveToHand(0, testFive);

    testPlayer.winHand(0);
    ASSERT_EQ(betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, Lose_function) {
    testPlayer.loseHand(0);
    ASSERT_EQ(-betForHand, testPlayer.getPurse());
}

TEST_F(playerTest, Double_down) {
    testPlayer.doubleDown(0);
    ASSERT_EQ(betForHand * 2, testPlayer.getHand(0).getBet());
}

TEST_F(playerTest, Dealer_bust_function_with_2_non_busted_hands) {
    testPlayer.newHand(betForHand);
    testPlayer.dealerBusted();
    ASSERT_EQ(betForHand * 2, testPlayer.getPurse());
}

TEST_F(playerTest, Dealer_bust_function_with_1_busted_hand) {
    testPlayer.moveToHand(0, testTen);
    testPlayer.moveToHand(0, testTen);
    testPlayer.moveToHand(0, testFive);
    testPlayer.newHand(betForHand);
    testPlayer.newHand(betForHand);
    testPlayer.moveToHand(2, testTen);

    testPlayer.dealerBusted();
    float expectedPurse = betForHand + betForHand - betForHand;
    ASSERT_EQ(expectedPurse, testPlayer.getPurse());
}

TEST_F(playerTest, Dealer_bust_function_with_1_blackjack_hand) {
    testPlayer.moveToHand(0, testTen);
    testPlayer.moveToHand(0, testAce);
    testPlayer.newHand(betForHand);

    testPlayer.dealerBusted();
    double expectedPurse = betForHand * 1.5 + betForHand;
    ASSERT_EQ(expectedPurse, testPlayer.getPurse());
}

TEST_F(playerTest, Surrender) {
    testPlayer.surrender(0);
    EXPECT_EQ(-betForHand * 0.5, testPlayer.getPurse());
}

TEST_F(playerTest, split_correct_hands) {
    using namespace testing;
    testPlayer.moveToHand(0, testAce);
    testPlayer.moveToHand(0, testFive);

    testPlayer.split(0);

    ASSERT_EQ(uint(2), testPlayer.getNumHands()) << "Player has 2 hands" << std::endl;


    EXPECT_EQ(uint(1), testPlayer.getHand(0).getNumCards());
    ASSERT_TRUE(testAce == testPlayer.getHand(0).getCard(0))
                                << "First hand should have first card of original hand" << std::endl;
    EXPECT_EQ(uint(1), testPlayer.getHand(1).getNumCards());
    ASSERT_TRUE(testFive == testPlayer.getHand(1).getCard(0)) << "Second hand should have second card" << std::endl;
}

TEST_F(playerTest, split_correct_bet){
    using namespace testing;
    testPlayer.moveToHand(0, testAce);
    testPlayer.moveToHand(0, testFive);

    testPlayer.split(0);

    ASSERT_EQ(uint(2), testPlayer.getNumHands()) << "Player has 2 hands" << std::endl;

    EXPECT_EQ(betForHand, testPlayer.getHand(0).getBet())
                        << "Each hand should have the starting bet value" << std::endl;
    EXPECT_EQ(betForHand , testPlayer.getHand(1).getBet())
                        << "Each hand should have the starting bet value" << std::endl;
}