//
// Created by richard on 7/21/19.
//

#include "../src/card.h"

#include "GUnit/GTest.h"
#include "GUnit/GMock.h"
#include "GUnit/GMake.h"
#include "GUnit/GSteps.h"
#include "GUnit.h"

#include "../src/card.h"

GSTEPS("Card.createNewCardWithRank") {
using namespace testing;

  Given("Create new card with rank {rank}"_step) = [&](int rank) {
    card testCard(rank);

    Then("Card value should be {value}") = [&](int value) {
      EXPECT_EQ(value, testCard.getValue());
    };

    Then("High Low Count should be {HighLowCount}, Zen Count should be {ZenCount} and Omega II Count should be {OmegaIICount}") =
        [&](int HighLowCount, int ZenCount, int OmegaIICount){
      EXPECT_EQ(HighLowCount, testCard.getHighLowCount());
      EXPECT_EQ(ZenCount, testCard.getZenCount());
      EXPECT_EQ(OmegaIICount, testCard.getOmegaIICount());
    };

    Then("Print String should be {PrintString}") = [&](const std::string& PrintString){
      EXPECT_EQ(PrintString, testCard.print());
    };

    Then("Card isAce {isAce}") = [&](bool isAce) {
      EXPECT_EQ(isAce, testCard.isAce());
    };
  };
}

GTEST("Card", "[Card should counts should be 0 when flipped face down]")
{
  card testCard(4);
  EXPECT_TRUE(testCard.isFaceUp());

  // Verify Counts are populated
  EXPECT_EQ(1, testCard.getHighLowCount());
  EXPECT_EQ(2, testCard.getZenCount());
  EXPECT_EQ(2, testCard.getOmegaIICount());

  testCard.flip();
  EXPECT_FALSE(testCard.isFaceUp());

  // Verify Counts are 0
  EXPECT_EQ(0, testCard.getHighLowCount());
  EXPECT_EQ(0, testCard.getZenCount());
  EXPECT_EQ(0, testCard.getOmegaIICount());

}
// TODO try to add normal GTest for testing flip function