#ifndef BJ_SIM_SHOE_H
#define BJ_SIM_SHOE_H

#include "sim/cards.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <utility>

namespace bj {

// A shoe of `numDecks` 52-card decks, pre-shuffled once so that dealing is an
// O(1) cursor advance and the dealt prefix IS the ordered deck history for free.
//
// `seen_` is the revealed-card histogram -- the sufficient statistic every
// counting system reads. Randomness is front-loaded into one Fisher-Yates per
// shuffle from a seedable engine, so a whole rollout is reproducible from a
// single seed and the per-card hot path draws zero random numbers.
//
// Storage is a fixed array sized for MaxDecks, so a Shoe is copyable as a plain
// value: copying one snapshots the entire rollout state (engine included).
template <int MaxDecks = 8>
class Shoe {
public:
    static constexpr int MAX_CARDS = 52 * MaxDecks;

    explicit Shoe(int numDecks = 6, double penetration = 0.75, std::uint64_t seed = 0)
        : n_(numDecks * 52),
          cut_(static_cast<int>(n_ * penetration)) {
        // Build the multiset once; shuffle() only permutes it and resets counters.
        int k = 0;
        for (int b = 0; b < NUM_BUCKETS; ++b) {
            full_[b] = PER_DECK[b] * numDecks;
            for (int i = 0; i < full_[b]; ++i) card_[k++] = static_cast<Card>(b);
        }
        reset(seed);
    }

    // Reseed and deal a fresh, freshly-shuffled shoe.
    void reset(std::uint64_t seed) {
        rng_.seed(seed);
        shuffle();
    }

    // Fisher-Yates over the active shoe; reset the deal cursor and revealed counts.
    // Called once per shoe (at a reshuffle), never per card.
    void shuffle() {
        for (int i = n_ - 1; i > 0; --i) {
            std::uniform_int_distribution<int> d(0, i);
            std::swap(card_[i], card_[d(rng_)]);
        }
        cursor_ = 0;
        for (int b = 0; b < NUM_BUCKETS; ++b) seen_[b] = 0;
    }

    // Hot path: draw the next card and reveal it (the common case).
    Card dealFaceUp() {
        Card c = card_[cursor_++];
        ++seen_[c];
        return c;
    }

    // Dealer hole card: drawn physically but not counted until reveal().
    Card dealHole() { return card_[cursor_++]; }
    void reveal(Card c) { ++seen_[c]; }

    // Reshuffle decision -- check once at the round boundary, never per card.
    bool needsShuffle() const { return cursor_ >= cut_; }

    int    cardsDealt()     const { return cursor_; }
    int    cardsRemaining() const { return n_ - cursor_; }
    double decksRemaining() const { return (n_ - cursor_) / 52.0; }

    int seen(Card b)      const { return seen_[b]; }
    int full(Card b)      const { return full_[b]; }
    int remaining(Card b) const { return full_[b] - seen_[b]; } // agent's view

    // Running count of any system: a dot product of the revealed histogram with a
    // weight table. True count divides by the (physical) decks remaining.
    int runningCount(const std::int8_t (&w)[NUM_BUCKETS]) const {
        int rc = 0;
        for (int b = 0; b < NUM_BUCKETS; ++b) rc += seen_[b] * w[b];
        return rc;
    }
    double trueCount(const std::int8_t (&w)[NUM_BUCKETS]) const {
        double decks = decksRemaining();
        return decks > 0.0 ? runningCount(w) / decks : 0.0;
    }

    // The ordered physical deal history, zero-copy.
    std::span<const Card> history() const {
        return {card_.data(), static_cast<std::size_t>(cursor_)};
    }

private:
    std::array<Card, MAX_CARDS> card_{};
    int n_;               // active number of cards (numDecks * 52)
    int cursor_ = 0;      // cards physically removed
    int cut_;             // reshuffle threshold (penetration point)
    int seen_[NUM_BUCKETS] = {}; // revealed-card histogram
    int full_[NUM_BUCKETS] = {}; // full composition (constant per config)
    std::mt19937_64 rng_;
};

} // namespace bj

#endif // BJ_SIM_SHOE_H
