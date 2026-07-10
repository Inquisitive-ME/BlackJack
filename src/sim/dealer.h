#ifndef BJ_SIM_DEALER_H
#define BJ_SIM_DEALER_H

#include "sim/hand.h"

namespace bj {

// Dealer drawing rule. Stands on any hard 17+; on a soft 17 it hits only under
// the "hit soft 17" (H17) rule, otherwise stands (S17). This is the whole dealer
// policy -- a pure predicate, no polymorphism.
constexpr bool dealerShouldHit(int total, bool soft, bool hitSoft17) {
    if (total < 17) return true;
    return total == 17 && soft && hitSoft17;
}

// Play the dealer's hand to completion, drawing face-up from the shoe.
// Templated on the shoe type so tests can drive it with a deterministic stack.
template <class ShoeT>
void dealerPlay(Hand &dealer, ShoeT &shoe, bool hitSoft17) {
    while (dealerShouldHit(dealer.total(), dealer.isSoft(), hitSoft17))
        dealer.add(shoe.dealFaceUp());
}

} // namespace bj

#endif // BJ_SIM_DEALER_H
