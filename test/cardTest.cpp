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

Given("Create new card with rank {rank}"_step) =
[&](int rank) {
  card testCard(rank);

Then("Card value should be {value}") =
[&](int value) {
EXPECT_EQ(value, testCard.getValue());
};
Then("Card isAce {isAce}") = [&](bool isAce)
{
  EXPECT_EQ(isAce, testCard.isAce());
};
};
}