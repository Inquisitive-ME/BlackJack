#ifndef BJ_SIM_PLAY_POLICY_H
#define BJ_SIM_PLAY_POLICY_H

#include "sim/cards.h"
#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/play_state.h"
#include "sim/round.h"
#include "sim/rules.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace bj {

inline Action playActionFromIndex(int a) {
    switch (a) {
        case 0: return Action::Hit;
        case 1: return Action::Stand;
        case 2: return Action::Double;
        case 3: return Action::Split;
        default: return Action::Surrender;
    }
}

// Play one round using a tabular play policy indexed by the discretized state and
// the High-Low true count. No insurance (the tabular agent learns only the five
// play actions). `bet` scales the wager; returns net chips.
template <class ShoeT>
double playRoundPolicy(ShoeT &shoe, const RulesConfig &rules, const std::int32_t *policy, double bet) {
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
    if (playerBJ || dealerBJ) {
        shoe.reveal(hole);
        if (playerBJ && dealerBJ) return 0.0;
        return playerBJ ? rules.blackjackPays * bet : -bet;
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
            bool first = ph.h.numCards() == 2;
            bool original = splits == 0;
            bool canSplit = first && ph.h.isPair() && splits < rules.maxSplits &&
                            (ph.h.pairCard() != ACE || rules.resplitAces || splits == 0);
            bool canDouble = first && (original || rules.doubleAfterSplit);
            bool canSurr = rules.surrenderAllowed && original && first && hands.size() == 1;

            double tc = shoe.trueCount(W_HILO);
            int idx = playStateIndex(ph.h.total(), ph.h.isSoft(), ph.h.isPair(), up, tc);
            Action a = playActionFromIndex(policy[idx]);

            // Mask illegal actions to a safe legal fallback.
            if ((a == Action::Split && !canSplit) || (a == Action::Double && !canDouble) ||
                (a == Action::Surrender && !canSurr))
                a = Action::Hit;

            if (a == Action::Split) {
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
            } else if (a == Action::Double) {
                ph.bet *= 2.0;
                ph.h.add(shoe.dealFaceUp());
                ph.done = true;
            } else if (a == Action::Surrender) {
                ph.surrendered = true;
                ph.done = true;
            } else if (a == Action::Hit) {
                ph.h.add(shoe.dealFaceUp());
                if (ph.h.isBusted()) ph.done = true;
            } else {
                ph.done = true; // Stand
            }
        }
    }

    shoe.reveal(hole);
    dealer.add(hole);
    bool anyLive = false;
    for (const PHand &ph : hands)
        if (!ph.surrendered && !ph.h.isBusted()) anyLive = true;
    if (anyLive) dealerPlay(dealer, shoe, rules.hitSoft17);

    double reward = 0.0;
    for (const PHand &ph : hands)
        reward += ph.surrendered ? -0.5 * ph.bet : settleHand(ph.h, dealer, ph.bet);
    return reward;
}

} // namespace bj

#endif // BJ_SIM_PLAY_POLICY_H
