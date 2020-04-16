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

std::string BJHand::print() const {
    std::string returnString;
    for (card mcard : cards) {
        if (mcard.isFaceUp())
            returnString.append(mcard.print() + " ");
    }
    returnString.append("Total: ");
    returnString.append(std::to_string(getTotal()));

    return returnString;
}

void BJHand::clear() {
    hand::clear();
    calculateTotal();
}

void BJHand::copy_to_hand(card cardToAdd) {
    hand::copy_to_hand(cardToAdd);
    calculateTotal();
}

void BJHand::move_to_hand(card &cardToMove) {
    hand::move_to_hand(cardToMove);
    calculateTotal();
}

void BJHand::give(int cardIndex, hand &handToGiveTo) {
    hand::give(cardIndex, handToGiveTo);
    calculateTotal();
}

void BJHand::flipCard(int cardIndex) {
    hand::flipCard(cardIndex);
    calculateTotal();
}

int BJHand::calculateTotal() {
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

int BJHand::getTotal() const {
    return total;
}

bool BJHand::isBusted() const {
    return (getTotal() > 21);
}

bool BJHand::isBlackJack() const {
    return (getTotal() == 21 && getNumCards() == 2);
}