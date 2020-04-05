//
// Created by richard on 12/31/19.
// From StackOverflow
// https://stackoverflow.com/questions/16491675/how-to-send-custom-message-in-google-c-testing-framework
//

#ifndef BLACKJACK_MYGTEST_H
#define BLACKJACK_MYGTEST_H

#include "gtest/gtest.h"

namespace testing
{
    namespace internal
    {
        enum class GTestColor { kDefault, kRed, kGreen, kYellow };
        extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
    }
}
#define GTEST_PRINTF(...)                                                   \
  do {                                                                \
    testing::internal::ColoredPrintf(testing::internal::GTestColor::kGreen,  \
                                     "[          ] ");                \
    testing::internal::ColoredPrintf(testing::internal::GTestColor::kYellow, \
                                     __VA_ARGS__);                    \
  } while (0)

// C++ stream interface
class GTestLog : public std::stringstream
{
public:
    ~GTestLog()
    {
        GTEST_PRINTF("%s",str().c_str());
    }
};

#define GTEST_LOG  GTestLog()

#endif //BLACKJACK_MYGTEST_H
