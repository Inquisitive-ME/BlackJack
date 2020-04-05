//
// Created by richard on 7/21/19.
//

#include "gtest/gtest.h"
#include "../src/card.h"

struct card_test
{
    int rank;
    int value;
    int HighLowCount;
    int ZenCount;
    int OmegaIICount;
    std::string PrintString;
    bool isAce;
};

class CardTest :
        public testing::TestWithParam<card_test> { };
/*
| rank          | value         | HighLowCount  | ZenCount      | OmegaIICount  | PrintString   | isAce         |
| 1             | 1             | -1            | -1            | 0             | A             | true          |
| 2             | 2             | 1             | 1             | 1             | 2             | false         |
| 3             | 3             | 1             | 1             | 1             | 3             | false         |
| 4             | 4             | 1             | 2             | 2             | 4             | false         |
| 5             | 5             | 1             | 2             | 2             | 5             | false         |
| 6             | 6             | 1             | 2             | 2             | 6             | false         |
| 7             | 7             | 0             | 1             | 1             | 7             | false         |
| 8             | 8             | 0             | 0             | 0             | 8             | false         |
| 9             | 9             | 0             | 0             | -1            | 9             | false         |
| 10            | 10            | -1            | -2            | -2            | 10            | false         |
| 11            | 10            | -1            | -2            | -2            | J             | false         |
| 12            | 10            | -1            | -2            | -2            | Q             | false         |
| 13            | 10            | -1            | -2            | -2            | K             | false         |
*/
INSTANTIATE_TEST_SUITE_P(all_card_values, CardTest,
                         testing::Values(card_test {1,  1,  -1, -1,  0, "A",  true},
                                         card_test {2,  2,   1,  1,  1, "2",  false},
                                         card_test {3,  3,   1,  1,  1, "3",  false},
                                         card_test {4,  4,   1,  2,  2, "4",  false},
                                         card_test {5,  5,   1,  2,  2, "5",  false},
                                         card_test {6,  6,   1,  2,  2, "6",  false},
                                         card_test {7,  7,   0,  1,  1, "7",  false},
                                         card_test {8,  8,   0,  0,  0, "8",  false},
                                         card_test {9,  9,   0,  0, -1, "9",  false},
                                         card_test {10, 10, -1, -2, -2, "10", false},
                                         card_test {11, 10, -1, -2, -2, "J" , false},
                                         card_test {12, 10, -1, -2, -2, "Q",  false},
                                         card_test {13, 10, -1, -2, -2, "K",  false})
                                         );


TEST_P(CardTest, Create_new_card_and_verify_value){
    card_test const& card_values = GetParam();
    card testCard(card_values.rank);

    ASSERT_EQ(card_values.value, testCard.getValue());
}

TEST_P(CardTest, Create_new_card_and_verify_counts)
{
    card_test const& card_values = GetParam();
    card testCard(card_values.rank);

    ASSERT_EQ(card_values.HighLowCount, testCard.getHighLowCount());
    ASSERT_EQ(card_values.ZenCount, testCard.getZenCount());
    ASSERT_EQ(card_values.OmegaIICount, testCard.getOmegaIICount());
}

TEST_P(CardTest, Create_new_card_and_verify_print)
{

    card_test const& card_values = GetParam();
    card testCard(card_values.rank);

    ASSERT_EQ(card_values.PrintString, testCard.print());
}

TEST_P(CardTest, Create_new_card_and_verify_isAce)
{
    card_test const& card_values = GetParam();
    card testCard(card_values.rank);

    ASSERT_EQ(card_values.isAce, testCard.isAce());
}

TEST(Card, Verify_flip_without_callback)
{
    card testCard(4);

    EXPECT_FALSE(testCard.isFaceUp());

    testCard.flip();
    ASSERT_TRUE(testCard.isFaceUp());
}

TEST(Card, Verify_flipCallback_is_called_when_card_is_flipped){
  int cardValue = 4;
  card testCard(cardValue);
  int testCallback = 0;

  testCard.setFlipCallBack([&](card passedCard){testCallback = passedCard.getValue();});

  EXPECT_FALSE(testCard.isFaceUp());
  EXPECT_NE(testCallback, cardValue);

  testCard.flip();
  ASSERT_TRUE(testCard.isFaceUp());
  ASSERT_EQ(testCallback, cardValue);
}

TEST(Card, Verify_flipCallBack_is_called_when_set_and_card_is_faceup)
{
  card testCard(4);
  //Make card faceup before setting callback
  testCard.flip();

  bool testCallback = false;
  testCard.setFlipCallBack([&testCallback](card passedCard){testCallback = true;});
  ASSERT_TRUE(testCallback);
}