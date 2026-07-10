#ifndef BJ_SIM_COUNT_PLAY_H
#define BJ_SIM_COUNT_PLAY_H

#include "sim/basic_strategy.h"
#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/round.h"
#include "sim/rules.h"

#include <cstddef>
#include <vector>

namespace bj {

// Basic strategy plus the classic count-based index deviations (the hit/stand/
// double subset of the "Illustrious 18"), keyed on the running true count. These
// are plays a flat-basic-strategy counter never makes -- they need the deck
// composition, which is exactly what the full observation provides.
inline Action countAwareStrategy(const Hand &h, Card up, const RulesConfig &r,
                                 bool cd, bool cs, bool csu, double tc) {
    if (!h.isSoft() && h.numCards() >= 2 && !h.isPair()) {
        const int t = h.total();
        const int d = (up == ACE) ? 11 : (up == TEN) ? 10 : up + 1;
        // Stand / hit index plays.
        if (t == 16 && d == 10 && tc >= 0.0) return Action::Stand;
        if (t == 16 && d == 9 && tc >= 5.0) return Action::Stand;
        if (t == 15 && d == 10 && tc >= 4.0) return Action::Stand;
        if (t == 13 && d == 2 && tc <= -1.0) return Action::Hit;
        if (t == 12 && d == 2 && tc >= 3.0) return Action::Stand;
        if (t == 12 && d == 3 && tc >= 2.0) return Action::Stand;
        if (t == 12 && d == 4 && tc < 0.0) return Action::Hit;
        // Double index plays (only when doubling is legal).
        if (cd) {
            if (t == 11 && d == 11 && tc >= 1.0) return Action::Double;
            if (t == 10 && d == 10 && tc >= 4.0) return Action::Double;
            if (t == 10 && d == 11 && tc >= 4.0) return Action::Double;
            if (t == 9 && d == 2 && tc >= 1.0) return Action::Double;
            if (t == 9 && d == 7 && tc >= 3.0) return Action::Double;
        }
    }
    return basicStrategy(h, up, r, cd, cs, csu);
}

// One round with count-aware play and insurance, for a `bet`-unit wager. `tc` is
// the running true count at bet time. When `deviations` is false this is exactly
// basic-strategy play (no insurance) -- the standard counter's baseline. Returns
// net chips including any insurance side bet.
template <class ShoeT>
double playRoundCounting(ShoeT &shoe, const RulesConfig &rules, double tc,
                         bool deviations, double insuranceTC, double bet) {
    Hand player, dealer;
    Card p1 = shoe.dealFaceUp(), up = shoe.dealFaceUp(), p2 = shoe.dealFaceUp();
    Card hole = shoe.dealHole();
    player.add(p1);
    player.add(p2);
    dealer.add(up);

    bool playerBJ = player.isBlackjack();
    bool dealerBJ = false;
    if (up == TEN || up == ACE) {
        Hand pk = dealer;
        pk.add(hole);
        dealerBJ = pk.isBlackjack();
    }

    // Insurance: offered on an Ace up-card; +EV exactly when the deck is ten-rich.
    double ins = 0.0;
    if (deviations && up == ACE && tc >= insuranceTC) {
        double stake = bet / 2.0;
        ins = dealerBJ ? +2.0 * stake : -stake;
    }

    if (playerBJ || dealerBJ) {
        shoe.reveal(hole);
        double main = (playerBJ && dealerBJ) ? 0.0 : (playerBJ ? rules.blackjackPays * bet : -bet);
        return main + ins;
    }

    struct PHand {
        Hand h;
        double bet;
        bool splitAce;
        bool surrendered;
        bool done;
    };
    std::vector<PHand> hands;
    hands.push_back(PHand{player, bet, false, false, false});
    int splits = 0;

    for (std::size_t i = 0; i < hands.size(); ++i) {
        while (!hands[i].done) {
            PHand &ph = hands[i];
            if (ph.h.isBusted()) { ph.done = true; break; }
            if (ph.splitAce) { ph.done = true; break; }
            bool firstDecision = ph.h.numCards() == 2;
            bool original = splits == 0;
            bool canSplit = firstDecision && ph.h.isPair() && splits < rules.maxSplits &&
                            (ph.h.pairCard() != ACE || rules.resplitAces || splits == 0);
            bool canDouble = firstDecision && (original || rules.doubleAfterSplit);
            bool canSurr = rules.surrenderAllowed && original && firstDecision && hands.size() == 1;
            Action a = deviations
                           ? countAwareStrategy(static_cast<const Hand &>(ph.h), up, rules,
                                                canDouble, canSplit, canSurr, tc)
                           : basicStrategy(static_cast<const Hand &>(ph.h), up, rules,
                                           canDouble, canSplit, canSurr);
            if (a == Action::Split && canSplit) {
                Card pc = ph.h.pairCard();
                bool ace = pc == ACE;
                double b = ph.bet;
                Hand h1;
                h1.add(pc);
                h1.add(shoe.dealFaceUp());
                Hand h2;
                h2.add(pc);
                h2.add(shoe.dealFaceUp());
                ph.h = h1;
                ph.splitAce = ace;
                hands.insert(hands.begin() + i + 1, PHand{h2, b, ace, false, ace});
                ++splits;
            } else if (a == Action::Double && canDouble) {
                ph.bet *= 2.0;
                ph.h.add(shoe.dealFaceUp());
                ph.done = true;
            } else if (a == Action::Surrender && canSurr) {
                ph.surrendered = true;
                ph.done = true;
            } else if (a == Action::Hit) {
                ph.h.add(shoe.dealFaceUp());
                if (ph.h.isBusted()) ph.done = true;
            } else {
                ph.done = true;
            }
        }
    }

    shoe.reveal(hole);
    dealer.add(hole);
    bool anyLive = false;
    for (const PHand &ph : hands)
        if (!ph.surrendered && !ph.h.isBusted()) anyLive = true;
    if (anyLive) dealerPlay(dealer, shoe, rules.hitSoft17);

    double reward = ins;
    for (const PHand &ph : hands)
        reward += ph.surrendered ? -0.5 * ph.bet : settleHand(ph.h, dealer, ph.bet);
    return reward;
}

} // namespace bj

#endif // BJ_SIM_COUNT_PLAY_H
