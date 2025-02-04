//
// Created by richard on 8/4/19.
//

#include "../src/bj_hand.h"
#include "../src/get_random_int.h"

#include "gtest/gtest.h"

class BJHandTest : public ::testing::Test {
public:
    int betForHand = 5;
    BJHand testHand;
    card testAce;
    card testFive;
    card testSeven;
    card testTen;

    BJHandTest() : testHand(betForHand), testAce(1), testFive(5), testSeven(7), testTen(10) {
        testAce.flip();
        testFive.flip();
        testSeven.flip();
        testTen.flip();
    }
};

TEST_F(BJHandTest, Creating_new_hand_has_bet_value) {
    ASSERT_EQ(betForHand, testHand.getBet());
}

// random number of cards not including Ace test total
TEST(BJHand, Add_random_number_of_cards_verify_count_and_total) {
    uint numCards = getRandomInt(0, 100);
    int expectedTotal = 0;
    BJHand testHand(0);

    for (uint i = 0; i < numCards; i++) {
        card testCard(getRandomInt(2, 13));
        testCard.flip();
        expectedTotal += testCard.getValue();
        testHand.copy_to_hand(testCard);
    }
    EXPECT_EQ(numCards, testHand.getNumCards());
    ASSERT_EQ(expectedTotal, testHand.getTotal()) << "Add " + std::to_string(numCards) + " cards to hand" << std::endl;

    testHand.clear();
    EXPECT_EQ(uint(0), testHand.getNumCards());
    ASSERT_EQ(0, testHand.getTotal()) << "Hand Cleared\n";
}

TEST_F(BJHandTest, Busted_hand_is_busted) {
    testHand.copy_to_hand(testTen);
    testHand.copy_to_hand(testFive);
    testHand.copy_to_hand(testSeven);

    ASSERT_TRUE(testHand.isBusted());
}

TEST_F(BJHandTest, Hand_with_ace_isSoft) {
    testHand.copy_to_hand(testAce);
    testHand.copy_to_hand(testSeven);

    EXPECT_EQ(18, testHand.getTotal());
    ASSERT_TRUE(testHand.isSoft());

    testHand.copy_to_hand(testSeven);

    EXPECT_EQ(15, testHand.getTotal());
    ASSERT_FALSE(testHand.isSoft());
}

TEST_F(BJHandTest, Blackjack_is_true) {
    testHand.copy_to_hand(testAce);
    testHand.copy_to_hand(testTen);

    ASSERT_TRUE(testHand.isBlackJack());
}

TEST_F(BJHandTest, BlackJack_is_false_with_ace_as_1) {
    testHand.copy_to_hand(testAce);
    testHand.copy_to_hand(testFive);
    testHand.copy_to_hand(testFive);

    ASSERT_FALSE(testHand.isBlackJack());
}

TEST_F(BJHandTest, BlackJack_is_false) {
    testHand.copy_to_hand(testFive);
    testHand.copy_to_hand(testFive);

    ASSERT_FALSE(testHand.isBlackJack());
}