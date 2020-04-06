//
// Created by richard on 8/4/19.
//

#ifndef BLACKJACK_BJHAND_H
#define BLACKJACK_BJHAND_H

#include "hand.h"

class BJHand : public hand {
private:
    float bet;
    int total;
    bool soft;

public:
    BJHand();

    BJHand(float bet);

    std::string print();

    int getTotal();

    bool isBusted();

    bool isBlackJack();

    float getBet() {
        return bet;
    }

    void doubleBet() {
        bet *= 2;
    }

    bool isSoft() {
        return soft;
    }
};


#endif //BLACKJACK_BJHAND_H
