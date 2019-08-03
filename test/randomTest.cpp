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
  int rangeMin = 0;
  int rangeMax = 9;
  int generatedRange=rangeMax - rangeMin + 1;
  double expected_accuracy = 0.005;

  int maxValue = rangeMin;
  int minValue = rangeMax;

  int count[10] = {0,0,0,0,0,0,0,0,0,0};
  for(int i = 0; i < numNumbersToGenerate; i++)
  {
    int test = getRandomInt(rangeMin, rangeMax);
    count[test] = count[test] + 1;

    if(test < minValue)
    {
      minValue = test;
    }

    if(test > maxValue)
    {
      maxValue = test;
    }
  }
  SHOULD("Have an even distribution")
  {
    for (int i = 0; i < rangeMax; i++)
    {
      EXPECT_LT(std::abs(count[i] - (numNumbersToGenerate / generatedRange)) / numNumbersToGenerate, expected_accuracy);
    }
  }
  SHOULD("Generate the minimum and maximum values of the given range")
  {
    EXPECT_EQ(minValue, rangeMin);
    EXPECT_EQ(maxValue, rangeMax);
  }


}