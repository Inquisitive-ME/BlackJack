//
// Created by richard on 8/4/19.
//

#include "../src/dealer_impl.h"

#include "gtest/gtest.h"

class dealerTest : public ::testing::Test {
public:
    DealerImpl testDealer;
    card testAce = card(1);
    card testFive = card(5);
    card testTen = card(10);

    void SetUp() override {
        testAce.flip();
        testFive.flip();
        testTen.flip();
    }

};

TEST_F(dealerTest, Dealer_is_hitting_when_hand_is_not_soft) {
    testDealer.copy_to_hand(testFive);
    testDealer.copy_to_hand(testTen);
    testDealer.copy_to_hand(testAce);

    ASSERT_TRUE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_not_hitting_when_hand_is_not_soft) {
    testDealer.copy_to_hand(testFive);
    testDealer.copy_to_hand(testTen);
    testDealer.copy_to_hand(testAce);
    testDealer.copy_to_hand(testAce);

    ASSERT_FALSE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_hitting_with_two_aces_one_soft) {
    testDealer.copy_to_hand(testFive);
    testDealer.copy_to_hand(testAce);
    testDealer.copy_to_hand(testAce);

    ASSERT_TRUE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_not_hitting_with_three_aces_one_soft) {
    testDealer.copy_to_hand(testFive);
    testDealer.copy_to_hand(testAce);
    testDealer.copy_to_hand(testAce);
    testDealer.copy_to_hand(testAce);

    ASSERT_FALSE(testDealer.isHitting());
}