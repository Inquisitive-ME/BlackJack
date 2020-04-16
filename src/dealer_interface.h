//
// Created by richard on 4/13/20.
//

#ifndef BLACKJACK_DEALER_INTERFACE_H
#define BLACKJACK_DEALER_INTERFACE_H

#include "bj_hand.h"
#include "deck.h"

class DealerInterface : virtual public BJHand {
public:
    virtual const bool isHitting() const = 0;

    virtual std::vector<card> getFaceUpCards() = 0;

    virtual ~DealerInterface() = default;
};

#endif //BLACKJACK_DEALER_INTERFACE_H
