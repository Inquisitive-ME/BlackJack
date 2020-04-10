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
    int calculateTotal();

public:
    BJHand();

    BJHand(float bet);

    std::string print() const;

    void clear() override;

    void add(card cardToAdd) override;

    void move_to_hand(card &cardToMove) override;

    void give(int cardIndex, hand &handToGiveTo) override;

    void flipCard(int cardIndex) override;

    int getTotal() const;

    bool isBusted() const;

    bool isBlackJack() const;

    float getBet() const{
        return bet;
    }

    void doubleBet() {
        bet *= 2;
    }

    bool isSoft() const{
        return soft;
    }
};


#endif //BLACKJACK_BJHAND_H
