#include "sim/play_state.h"

#include "gtest/gtest.h"

#include <set>

using namespace bj;

TEST(PlayState, IndicesAreAUniqueBijectionInRange) {
    std::set<int> seen;
    for (int total = PS_TOTAL_MIN; total <= PS_TOTAL_MAX; ++total)
        for (int soft = 0; soft < 2; ++soft)
            for (int pair = 0; pair < 2; ++pair)
                for (int up = 0; up < NUM_BUCKETS; ++up)
                    for (int tc = PS_TC_MIN; tc <= PS_TC_MAX; ++tc) {
                        int idx = playStateIndex(total, soft, pair, up, double(tc));
                        ASSERT_GE(idx, 0);
                        ASSERT_LT(idx, NUM_PLAY_STATES);
                        ASSERT_TRUE(seen.insert(idx).second) << "duplicate index " << idx;
                    }
    EXPECT_EQ(int(seen.size()), NUM_PLAY_STATES);
}

TEST(PlayState, TrueCountBinsClampAndRound) {
    EXPECT_EQ(psTcBin(-100.0), 0);
    EXPECT_EQ(psTcBin(100.0), PS_TC_BINS - 1);
    EXPECT_EQ(psTcBin(0.0), -PS_TC_MIN);        // tc 0
    EXPECT_EQ(psTcBin(0.4), -PS_TC_MIN);        // rounds to 0
    EXPECT_EQ(psTcBin(0.6), -PS_TC_MIN + 1);    // rounds to 1
    EXPECT_EQ(psTcBin(-2.0), -PS_TC_MIN - 2);   // tc -2
}

TEST(PlayState, TotalClampsOutOfRange) {
    // Below/above the tracked range collapse onto the end bins, still in range.
    for (double tc : {-4.0, 0.0, 4.0}) {
        EXPECT_EQ(playStateIndex(2, false, false, 5, tc),
                  playStateIndex(PS_TOTAL_MIN, false, false, 5, tc));
        EXPECT_EQ(playStateIndex(25, false, false, 5, tc),
                  playStateIndex(PS_TOTAL_MAX, false, false, 5, tc));
    }
}
