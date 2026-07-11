#ifndef BJ_SIM_BASIC_STRATEGY_H
#define BJ_SIM_BASIC_STRATEGY_H

#include "sim/cards.h"
#include "sim/hand.h"
#include "sim/rules.h"

namespace bj {

// Multi-deck, H17 basic strategy, expressed over the abstract hand state so it
// can be queried without building a Hand (used to seed the RL base policy).
inline Action basicStrategyAbstract(int total, bool soft, Card pairCard, Card up,
                                    bool canDouble, bool canSplit, bool canSurrender) {
    // Dealer up-card value with Ace as 11.
    const int d = (up == ACE) ? 11 : (up == TEN) ? 10 : up + 1;
    const int t = total;

    // Pairs.
    if (canSplit) {
        const Card pc = pairCard;
        const int pv = (pc == ACE) ? 11 : VALUE[pc]; // ten-group -> 10
        switch (pv) {
            case 11: return Action::Split;                                    // A,A
            case 9:  if (d != 7 && d != 10 && d != 11) return Action::Split; break; // vs 2-6,8,9
            case 8:  return Action::Split;                                    // 8,8
            case 7:  if (d <= 7) return Action::Split; break;
            case 6:  if (d <= 6) return Action::Split; break;
            case 4:  if (d == 5 || d == 6) return Action::Split; break;
            case 3:  if (d <= 7) return Action::Split; break;
            case 2:  if (d <= 7) return Action::Split; break;
            default: break;                                                   // 10,10 and 5,5: never
        }
        // fall through: treat like the equivalent hard/soft total
    }

    // Soft totals (an ace counted as 11).
    if (soft) {
        if (t >= 20) return Action::Stand;
        if (t == 19) return (canDouble && d == 6) ? Action::Double : Action::Stand;
        if (t == 18) {
            if (canDouble && d >= 3 && d <= 6) return Action::Double;
            return (d == 2 || d == 7 || d == 8) ? Action::Stand : Action::Hit;
        }
        if (t == 17) return (canDouble && d >= 3 && d <= 6) ? Action::Double : Action::Hit;
        if (t == 15 || t == 16) return (canDouble && d >= 4 && d <= 6) ? Action::Double : Action::Hit;
        if (t == 13 || t == 14) return (canDouble && (d == 5 || d == 6)) ? Action::Double : Action::Hit;
        return Action::Hit;
    }

    // Hard totals -- late surrender first (H17).
    if (canSurrender) {
        if (t == 16 && (d == 9 || d == 10 || d == 11)) return Action::Surrender;
        if (t == 15 && (d == 10 || d == 11)) return Action::Surrender;
        if (t == 17 && d == 11) return Action::Surrender;
    }
    if (t >= 17) return Action::Stand;
    if (t >= 13 && t <= 16) return (d >= 2 && d <= 6) ? Action::Stand : Action::Hit;
    if (t == 12) return (d >= 4 && d <= 6) ? Action::Stand : Action::Hit;
    if (t == 11) return canDouble ? Action::Double : Action::Hit;
    if (t == 10) return (canDouble && d <= 9) ? Action::Double : Action::Hit;
    if (t == 9)  return (canDouble && d >= 3 && d <= 6) ? Action::Double : Action::Hit;
    return Action::Hit; // 5-8
}

// Hand-based wrapper (the reference play policy used to validate the engine).
inline Action basicStrategy(const Hand &h, Card up, const RulesConfig &,
                            bool canDouble, bool canSplit, bool canSurrender) {
    return basicStrategyAbstract(h.total(), h.isSoft(), h.pairCard(), up,
                                 canDouble, canSplit, canSurrender);
}

} // namespace bj

#endif // BJ_SIM_BASIC_STRATEGY_H
