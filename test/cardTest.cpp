//
// Created by richard on 7/21/19.
//

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

GTEST("Card", "Verify flipCallback is called when first assigned and card is flipped")
{
  card testCard(4);
  bool testCallback = false;

  testCard.setFlipCallBack([&](card passedCard){testCallback = passedCard.isFaceUp();});

  EXPECT_FALSE(testCard.isFaceUp());
  EXPECT_FALSE(testCallback);

  testCard.flip();
  EXPECT_TRUE(testCard.isFaceUp());
  EXPECT_TRUE(testCallback);

}