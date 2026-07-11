#ifndef BJ_SIM_PLAY_STATE_H
#define BJ_SIM_PLAY_STATE_H

#include "sim/cards.h"

#include <cmath>

namespace bj {

// Discretized play state for tabular reinforcement learning:
//   (player total, soft, pair, dealer up-card bucket, High-Low true-count bin).
// The same indexing is mirrored in the Python trainer so a learned policy table
// can be scored in C++.
inline constexpr int PS_TOTAL_MIN = 4;
inline constexpr int PS_TOTAL_MAX = 21;
inline constexpr int PS_TOTAL_BINS = PS_TOTAL_MAX - PS_TOTAL_MIN + 1; // 18
inline constexpr int PS_TC_MIN = -5;
inline constexpr int PS_TC_MAX = 5;
inline constexpr int PS_TC_BINS = PS_TC_MAX - PS_TC_MIN + 1;          // 11
inline constexpr int NUM_PLAY_ACTIONS = 5;                           // hit,stand,double,split,surrender
inline constexpr int NUM_PLAY_STATES = PS_TOTAL_BINS * 2 * 2 * NUM_BUCKETS * PS_TC_BINS; // 7920

inline int psTcBin(double tc) {
    long t = std::lround(tc);
    if (t < PS_TC_MIN) t = PS_TC_MIN;
    if (t > PS_TC_MAX) t = PS_TC_MAX;
    return static_cast<int>(t - PS_TC_MIN);
}

inline int playStateIndex(int total, bool soft, bool pair, int dealerUp, double tc) {
    int ti = total < PS_TOTAL_MIN ? 0
             : total > PS_TOTAL_MAX ? PS_TOTAL_BINS - 1
                                    : total - PS_TOTAL_MIN;
    int idx = ti;
    idx = idx * 2 + (soft ? 1 : 0);
    idx = idx * 2 + (pair ? 1 : 0);
    idx = idx * NUM_BUCKETS + dealerUp;
    idx = idx * PS_TC_BINS + psTcBin(tc);
    return idx;
}

} // namespace bj

#endif // BJ_SIM_PLAY_STATE_H
