//
// Created by richard on 7/30/19.
//

#include "../src/getRandomInt.h"
#include <cmath>
#include <iostream>

#include "GUnit/GTest.h"

GTEST("getRandomInt", "Verify uniform distribution of getRandomInt")
{
  double numNumbersToGenerate = 100000;
  int rangeToGenerate = 10;
  double expected_accuracy = 0.005;

  int count[10] = {0,0,0,0,0,0,0,0,0,0};
  for(int i = 0; i < numNumbersToGenerate; i++)
  {
    int test = getRandomInt(0, rangeToGenerate);
    count[test] = count[test] + 1;
  }
  for(int i = 0; i < rangeToGenerate; i++)
  {
    EXPECT_LT(std::abs(count[i] - (numNumbersToGenerate / rangeToGenerate))/numNumbersToGenerate, expected_accuracy);
  }

}