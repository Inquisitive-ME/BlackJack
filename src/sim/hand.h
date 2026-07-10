#ifndef BJ_SIM_HAND_H
#define BJ_SIM_HAND_H

#include "sim/cards.h"

namespace bj {

// A blackjack hand with an O(1) incremental total. Adding a card is a single
// add + a couple of branches; the soft-ace promotion (+10) is derived on read,
// never by rescanning the cards (the legacy BJHand recomputed the whole hand on
// every add, which was O(n) per add / O(n^2) per hand).
//
// The hand keeps only the aggregates the engine and observation need -- running
// hard total, ace count, card count, and whether the first two cards form a
// splittable pair -- not the individual cards. The deck history lives in Shoe.
class Hand {
public:
    void clear() {
        hard_ = 0;
        aces_ = 0;
        cards_ = 0;
        firstCard_ = 0;
        pair_ = false;
    }

    void add(Card c) {
        hard_ += VALUE[c];              // every ace counts as 1 here
        aces_ += (c == ACE);
        if (cards_ == 0) firstCard_ = c;
        else if (cards_ == 1) pair_ = (c == firstCard_);
        ++cards_;
    }

    // Best total <= 21 when possible: promote a single ace from 1 to 11 if it
    // fits. At most one ace is ever promoted (two 11s would bust).
    int total() const {
        return (aces_ > 0 && hard_ + 10 <= 21) ? hard_ + 10 : hard_;
    }

    // An ace is currently counted as 11.
    bool isSoft() const { return aces_ > 0 && hard_ + 10 <= 21; }

    // hard_ is the minimal possible total, so busting is decided by it alone.
    bool isBusted() const { return hard_ > 21; }

    // A two-card 21. (Payout context -- e.g. excluding split hands -- is the
    // engine's concern in a later phase; this is the pure predicate.)
    bool isBlackjack() const { return cards_ == 2 && total() == 21; }

    // Exactly two cards of the same rank-bucket (any two ten-valued cards pair).
    bool isPair() const { return cards_ == 2 && pair_; }
    Card pairCard() const { return firstCard_; } // valid when isPair()

    int numCards() const { return cards_; }

private:
    int  hard_ = 0;      // total with every ace as 1
    int  aces_ = 0;
    int  cards_ = 0;
    Card firstCard_ = 0;
    bool pair_ = false;
};

} // namespace bj

#endif // BJ_SIM_HAND_H
