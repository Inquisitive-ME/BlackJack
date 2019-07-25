//
// Created by richard on 7/21/19.
//

#include "../src/card.h"

#include "GUnit/GTest.h"
#include "GUnit/GMock.h"
#include "GUnit/GMake.h"
#include "GUnit/GSteps.h"
#include "GUnit.h"


TEST(cardTest, FLIP) {
  card testCard = card();
  ASSERT_FALSE(testCard.isFaceUp());
  testCard.flip();
  ASSERT_TRUE(testCard.isFaceUp());
}