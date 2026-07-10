#ifndef BJ_SIM_ROUND_H
#define BJ_SIM_ROUND_H

#include "sim/cards.h"
#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/rules.h"

#include <cstddef>
#include <vector>

namespace bj {

// Net settlement of one finished, non-natural player hand versus the dealer,
// in units of `bet` (2.0 for a doubled hand).
inline double settleHand(const Hand &player, const Hand &dealer, double bet) {
    if (player.isBusted()) return -bet;      // player bust loses even if dealer also busts
    if (dealer.isBusted()) return +bet;
    int p = player.total(), d = dealer.total();
    if (p > d) return +bet;
    if (p < d) return -bet;
    return 0.0;                              // push
}

// Play one full round for a single player against the dealer, choosing actions
// via `policy`. Returns net reward in units of the base (1.0) bet.
//
// `policy` is called as:
//   Action policy(const Hand& hand, Card dealerUp, const RulesConfig&,
//                 bool canDouble, bool canSplit, bool canSurrender);
// An action the flags say is illegal is treated as Stand (masked, never looped).
//
// Fixes over the legacy game loop: dealer blackjack is peeked on a 10/Ace up
// card and ends the round; a natural pays `blackjackPays` and pushes vs a dealer
// natural; double takes exactly one card and is terminal; split deals a second
// card to *each* new hand; split aces take one card and stand; surrender is
// terminal (-0.5) and settled once.
template <class ShoeT, class Policy>
double playRound(ShoeT &shoe, const RulesConfig &rules, Policy &&policy) {
    Hand player, dealer;
    Card p1 = shoe.dealFaceUp();
    Card up = shoe.dealFaceUp();     // dealer up card (revealed)
    Card p2 = shoe.dealFaceUp();
    Card hole = shoe.dealHole();     // dealer hole card (not counted until shown)
    player.add(p1);
    player.add(p2);
    dealer.add(up);

    // Naturals. The dealer only peeks for blackjack on a ten or ace up card.
    bool playerBJ = player.isBlackjack();
    bool dealerBJ = false;
    if (up == TEN || up == ACE) {
        Hand peek = dealer;
        peek.add(hole);
        dealerBJ = peek.isBlackjack();
    }
    if (playerBJ || dealerBJ) {
        shoe.reveal(hole);
        if (playerBJ && dealerBJ) return 0.0;          // push
        return playerBJ ? rules.blackjackPays : -1.0;  // 3:2, or dealer natural
    }

    struct PHand {
        Hand h;
        double bet;
        bool splitAce;
        bool surrendered;
        bool done;
    };
    std::vector<PHand> hands;
    hands.push_back(PHand{player, 1.0, false, false, false});
    int splits = 0;

    for (std::size_t i = 0; i < hands.size(); ++i) {
        while (!hands[i].done) {
            PHand &ph = hands[i];
            if (ph.h.isBusted()) { ph.done = true; break; }
            if (ph.splitAce)     { ph.done = true; break; } // split ace: one card, stand

            bool firstDecision = (ph.h.numCards() == 2);
            bool original = (splits == 0);
            bool canSplit = firstDecision && ph.h.isPair() && splits < rules.maxSplits &&
                            (ph.h.pairCard() != ACE || rules.resplitAces || splits == 0);
            bool canDouble = firstDecision && (original || rules.doubleAfterSplit);
            bool canSurrender = rules.surrenderAllowed && original && firstDecision && hands.size() == 1;

            Action a = policy(static_cast<const Hand &>(ph.h), up, rules,
                              canDouble, canSplit, canSurrender);

            if (a == Action::Split && canSplit) {
                Card pc = ph.h.pairCard();
                bool ace = (pc == ACE);
                double bet = ph.bet;
                Hand h1;
                h1.add(pc);
                h1.add(shoe.dealFaceUp());
                Hand h2;
                h2.add(pc);
                h2.add(shoe.dealFaceUp());
                ph.h = h1;
                ph.splitAce = ace;
                hands.insert(hands.begin() + i + 1, PHand{h2, bet, ace, false, false});
                ++splits;
                // `ph` is now dangling after the insert; the loop re-reads hands[i].
            } else if (a == Action::Double && canDouble) {
                ph.bet *= 2.0;
                ph.h.add(shoe.dealFaceUp());
                ph.done = true;                       // exactly one card, terminal
            } else if (a == Action::Surrender && canSurrender) {
                ph.surrendered = true;
                ph.done = true;
            } else if (a == Action::Hit) {
                ph.h.add(shoe.dealFaceUp());
            } else {
                ph.done = true;                       // Stand, or a masked illegal action
            }
        }
    }

    // Showdown: reveal the hole card, and let the dealer draw only if some hand
    // can still win.
    shoe.reveal(hole);
    dealer.add(hole);
    bool anyLive = false;
    for (const PHand &ph : hands)
        if (!ph.surrendered && !ph.h.isBusted()) anyLive = true;
    if (anyLive) dealerPlay(dealer, shoe, rules.hitSoft17);

    double reward = 0.0;
    for (const PHand &ph : hands)
        reward += ph.surrendered ? -0.5 : settleHand(ph.h, dealer, ph.bet);
    return reward;
}

} // namespace bj

#endif // BJ_SIM_ROUND_H
