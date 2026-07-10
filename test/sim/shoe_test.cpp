#include "sim/shoe.h"

#include "gtest/gtest.h"

#include <map>

using bj::Card;
using bj::NUM_BUCKETS;
using bj::Shoe;

namespace {
int totalCards(int decks) { return decks * 52; }
}

TEST(Shoe, FreshShoeHasFullComposition) {
    Shoe<> shoe(6, 0.75, 1);
    int sum = 0;
    for (int b = 0; b < NUM_BUCKETS; ++b) {
        EXPECT_EQ(shoe.full(b), int(bj::PER_DECK[b]) * 6);
        EXPECT_EQ(shoe.remaining(b), int(bj::PER_DECK[b]) * 6); // nothing revealed yet
        sum += shoe.full(b);
    }
    EXPECT_EQ(sum, totalCards(6));
    EXPECT_EQ(shoe.cardsRemaining(), totalCards(6));
    EXPECT_EQ(shoe.cardsDealt(), 0);
}

TEST(Shoe, DealingIsAConservingPermutation) {
    // Deal the entire shoe face-up; every card appears exactly its full count.
    Shoe<> shoe(2, 1.0, 7); // penetration 1.0 so all cards are dealt without reshuffle
    const int N = totalCards(2);
    std::map<Card, int> tally;
    for (int i = 0; i < N; ++i) tally[shoe.dealFaceUp()]++;

    EXPECT_EQ(shoe.cardsRemaining(), 0);
    for (int b = 0; b < NUM_BUCKETS; ++b) {
        EXPECT_EQ(tally[Card(b)], int(bj::PER_DECK[b]) * 2) << "bucket " << b;
        EXPECT_EQ(shoe.seen(b), int(bj::PER_DECK[b]) * 2);
        EXPECT_EQ(shoe.remaining(b), 0);
    }
}

TEST(Shoe, HistoryPrefixMatchesRevealedCounts) {
    Shoe<> shoe(1, 1.0, 3);
    for (int i = 0; i < 20; ++i) shoe.dealFaceUp();
    int recount[NUM_BUCKETS] = {};
    for (Card c : shoe.history()) recount[c]++;
    EXPECT_EQ(int(shoe.history().size()), 20);
    for (int b = 0; b < NUM_BUCKETS; ++b) EXPECT_EQ(recount[b], shoe.seen(b));
}

TEST(Shoe, SameSeedReproducesDifferentSeedDiverges) {
    Shoe<> a(6, 0.75, 42), b(6, 0.75, 42), c(6, 0.75, 7);
    bool ab_same = true, ac_diff = false;
    for (int i = 0; i < 100; ++i) {
        Card ca = a.dealFaceUp(), cb = b.dealFaceUp(), cc = c.dealFaceUp();
        ab_same &= (ca == cb);
        ac_diff |= (ca != cc);
    }
    EXPECT_TRUE(ab_same);
    EXPECT_TRUE(ac_diff);
}

TEST(Shoe, CopyIsAnIndependentSnapshot) {
    Shoe<> a(6, 0.75, 99);
    for (int i = 0; i < 30; ++i) a.dealFaceUp();
    Shoe<> b = a; // snapshot mid-shoe (engine state included)
    for (int i = 0; i < 20; ++i) EXPECT_EQ(a.dealFaceUp(), b.dealFaceUp());
    EXPECT_EQ(a.cardsDealt(), b.cardsDealt());
}

TEST(Shoe, BalancedCountsSumToZeroOverAFullShoe) {
    // High-Low, Zen and Omega-II are all balanced: revealing every card nets zero.
    Shoe<> shoe(4, 1.0, 11);
    const int N = totalCards(4);
    for (int i = 0; i < N; ++i) shoe.dealFaceUp();
    EXPECT_EQ(shoe.runningCount(bj::W_HILO), 0);
    EXPECT_EQ(shoe.runningCount(bj::W_ZEN), 0);
    EXPECT_EQ(shoe.runningCount(bj::W_OMEGA), 0);
}

TEST(Shoe, CountTablesAreStandard) {
    // Pin the count tables against the published values (independent literals), so
    // a transcription typo in the header is caught. Buckets: A,2,3,4,5,6,7,8,9,ten.
    const int value[NUM_BUCKETS] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const int hilo[NUM_BUCKETS]  = {-1, 1, 1, 1, 1, 1, 0, 0, 0, -1};
    const int zen[NUM_BUCKETS]   = {-1, 1, 1, 2, 2, 2, 1, 0, 0, -2};
    const int omega[NUM_BUCKETS] = {0, 1, 1, 2, 2, 2, 1, 0, -1, -2};
    for (int b = 0; b < NUM_BUCKETS; ++b) {
        EXPECT_EQ(int(bj::VALUE[b]), value[b]) << "value bucket " << b;
        EXPECT_EQ(int(bj::W_HILO[b]), hilo[b]) << "hilo bucket " << b;
        EXPECT_EQ(int(bj::W_ZEN[b]), zen[b]) << "zen bucket " << b;
        EXPECT_EQ(int(bj::W_OMEGA[b]), omega[b]) << "omega bucket " << b;
    }
    for (int r = 1; r <= 13; ++r) {
        Card expect = r == 1 ? 0 : r <= 9 ? Card(r - 1) : Card(9);
        EXPECT_EQ(bj::bucketOfRank(r), expect);
    }
}

TEST(Shoe, ReshuffleTriggersAtPenetration) {
    Shoe<> shoe(1, 0.5, 5); // 52 cards, cut at 26
    EXPECT_FALSE(shoe.needsShuffle());
    for (int i = 0; i < 25; ++i) {
        shoe.dealFaceUp();
        EXPECT_FALSE(shoe.needsShuffle());
    }
    shoe.dealFaceUp(); // 26th
    EXPECT_TRUE(shoe.needsShuffle());
    shoe.shuffle();
    EXPECT_FALSE(shoe.needsShuffle());
    EXPECT_EQ(shoe.cardsDealt(), 0);
}

TEST(Shoe, HoleCardIsNotCountedUntilRevealed) {
    Shoe<> shoe(6, 0.75, 8);
    Card hole = shoe.dealHole();
    EXPECT_EQ(shoe.seen(hole), 0) << "hole card must not be counted before reveal";
    int before = shoe.runningCount(bj::W_HILO);
    shoe.reveal(hole);
    EXPECT_EQ(shoe.seen(hole), 1);
    EXPECT_EQ(shoe.runningCount(bj::W_HILO), before + int(bj::W_HILO[hole]));
}
