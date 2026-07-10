#ifndef BJ_SIM_CARDS_H
#define BJ_SIM_CARDS_H

#include <cstdint>

namespace bj {

// A card is a rank-class "bucket" -- the minimal sufficient statistic for
// blackjack EV and for every classic counting system:
//   0      = Ace
//   1..8   = ranks 2..9
//   9      = ten-group (10, J, Q, K)
using Card = std::uint8_t;

inline constexpr int NUM_BUCKETS = 10;

// Blackjack value of each bucket (Ace as 1; the soft +10 promotion lives in Hand).
inline constexpr std::int8_t VALUE[NUM_BUCKETS]    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Cards of each bucket in a single 52-card deck (four per rank, sixteen tens).
inline constexpr std::int8_t PER_DECK[NUM_BUCKETS] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 16};

// Counting-system weights, transcribed from the legacy card.cpp and pinned by
// test/sim/shoe_test.cpp against a legacy `card` for every bucket. Any counting
// system is a dot product of these against the revealed-card histogram.
inline constexpr std::int8_t W_HILO[NUM_BUCKETS]   = {-1,  1,  1,  1,  1,  1,  0,  0,  0, -1};
inline constexpr std::int8_t W_ZEN[NUM_BUCKETS]    = {-1,  1,  1,  2,  2,  2,  1,  0,  0, -2};
inline constexpr std::int8_t W_OMEGA[NUM_BUCKETS]  = { 0,  1,  1,  2,  2,  2,  1,  0, -1, -2};

// Map a legacy rank (1 = Ace .. 13 = King) to its bucket.
constexpr Card bucketOfRank(int rank) {
    return rank == 1 ? Card(0) : rank <= 9 ? Card(rank - 1) : Card(9);
}

} // namespace bj

#endif // BJ_SIM_CARDS_H
