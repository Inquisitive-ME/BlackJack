//
// Created by richard on 7/21/19.
//

#include <limits.h>
#include "gtest/gtest.h"

// Returns n! (the factorial of n).  For negative n, n! is defined to be 1.
int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }

  return result;
}


// Tests factorial of 0.
TEST(FactorialTest, Zero) {
EXPECT_EQ(1, Factorial(0));
}