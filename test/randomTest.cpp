//
// Created by richard on 7/30/19.
//

#include "../src/get_random_int.h"
#include <cmath>
#include <iostream>

#include "gtest/gtest.h"

TEST(getRandomInt, Verify_uniform_distribution_of_getRandomInt) {
    double numNumbersToGenerate = 100000;
    int rangeMin = 0;
    int rangeMax = 9;
    int generatedRange = rangeMax - rangeMin + 1;
    double expected_accuracy = 0.005;

    int maxValue = rangeMin;
    int minValue = rangeMax;

    int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < numNumbersToGenerate; i++) {
        int test = getRandomInt(rangeMin, rangeMax);
        count[test] = count[test] + 1;

        if (test < minValue) {
            minValue = test;
        }

        if (test > maxValue) {
            maxValue = test;
        }
    }

    for (int i = 0; i < rangeMax; i++) {
        ASSERT_LT(std::abs(count[i] - (numNumbersToGenerate / generatedRange)) / numNumbersToGenerate,
                  expected_accuracy) << "Verify Even Distribution" << std::endl;
    }

    ASSERT_EQ(minValue, rangeMin) << "Verify the minimum values were generated" << std::endl;
    ASSERT_EQ(maxValue, rangeMax) << "Verify the maximum values were generated" << std::endl;

}