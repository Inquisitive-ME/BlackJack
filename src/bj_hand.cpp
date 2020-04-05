//
// Created by richard on 8/4/19.
//

#include "bj_hand.h"

BJHand::BJHand() {
    bet = 0;
    total = 0;
    soft = false;
}

BJHand::BJHand(float bet) {
    this->bet = bet;
    total = 0;
    soft = false;
}

std::string BJHand::print() {
    std::string returnString;
    for (card mcard : cards) {
        if (mcard.isFaceUp())
            returnString.append(mcard.print());
    }
    returnString.append(" Total: ");
    returnString.append(std::to_string(getTotal()));

    return returnString;
}

int BJHand::getTotal() {
    total = 0;
    bool hasAce = false;
    for (card mcard : cards) {
        if (mcard.isFaceUp()) {
            if (mcard.isAce()) {
                hasAce = true;
            }
            total += mcard.getValue();
        }
    }
    if (hasAce && total <= 11) {
        soft = true;
        total += 10;
    } else
        soft = false;
    return total;
}

bool BJHand::isBusted() {
    return (getTotal() > 21);
}

bool BJHand::isBlackJack() {
    return (getTotal() == 21 && getNumCards() == 2);
}