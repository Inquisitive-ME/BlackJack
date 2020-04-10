/*
 * hand.h
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */

#ifndef HAND_H_
#define HAND_H_

#include <iostream>
#include <vector>
#include "card.h"

class hand {
protected:
    std::vector<card> cards;

public:
    hand();

    virtual void clear();

    virtual void add(card cardToAdd);

    virtual void move_to_hand(card &cardToMove);

    virtual void give(int cardIndex, hand &handToGiveTo);

    uint getNumCards() const;

    const card &getCard(int cardIndex) const;

    virtual void flipCard(int cardIndex);

    virtual bool operator==(const hand &otherHand) const { return true; };

    virtual ~hand() = default;
};


#endif /* HAND_H_ */