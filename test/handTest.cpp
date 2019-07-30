//
// Created by richard on 7/30/19.
//

#include "../src/hand.h"

#include "GUnit/GTest.h"

//TODO get Total Feature test to do different combinations


GTEST("Hand", "Creating new hand has bet value")
{
  hand testHand(5);
  EXPECT_EQ(5, testHand.getBet());

}