//
// Created by richard on 8/4/19.
//

#include "../src/dealer.h"

#include "gtest/gtest.h"

class dealerTest : public ::testing::Test {
public:
    dealer testDealer;
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
    testDealer.add(testFive);
    testDealer.add(testTen);
    testDealer.add(testAce);

    ASSERT_TRUE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_not_hitting_when_hand_is_not_soft) {
    testDealer.add(testFive);
    testDealer.add(testTen);
    testDealer.add(testAce);
    testDealer.add(testAce);

    ASSERT_FALSE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_hitting_with_two_aces_one_soft) {
    testDealer.add(testFive);
    testDealer.add(testAce);
    testDealer.add(testAce);

    ASSERT_TRUE(testDealer.isHitting());
}

TEST_F(dealerTest, Dealer_is_not_hitting_with_three_aces_one_soft) {
    testDealer.add(testFive);
    testDealer.add(testAce);
    testDealer.add(testAce);
    testDealer.add(testAce);

    ASSERT_FALSE(testDealer.isHitting());
}