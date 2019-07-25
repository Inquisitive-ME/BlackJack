//
// Created by richard on 7/21/19.
//

#include "../src/card.h"

#include "GUnit/GTest.h"
#include "GUnit/GMock.h"
#include "GUnit/GMake.h"
#include "GUnit/GSteps.h"
#include "GUnit.h"

GSTEPS("Card.createNewCardWithRank")
{
  using namespace testing;
  card testCard;

  Given("Create new card with rank {rank}"_step) = [&](int rank)
  {
    testCard = card(rank);
  };
  Then("Card value should be {value}"_step) = [&](int value)
  {
    EXPECT_EQ(testCard.getValue(), value);
  };
}

//TEST(cardTest, FLIP) {
//  card testCard = card();
//  ASSERT_FALSE(testCard.isFaceUp());
//  testCard.flip();
//  ASSERT_TRUE(testCard.isFaceUp());
//}