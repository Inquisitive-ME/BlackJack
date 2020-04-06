/*
 * BJDealer.h
 *
 *  Created on: May 23, 2018
 *      Author: richard
 */

#ifndef DEALER_H_
#define DEALER_H_

#include "bj_hand.h"
#include "deck.h"

class dealer : public BJHand {
public:
    dealer();

    bool isHitting();

    std::vector<card> getFaceUpCards();

    virtual ~dealer();
};

#endif /* DEALER_H_ */
