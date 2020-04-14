/*
 * BJDealer.cpp
 *
 *  Created on: May 23, 2018
 *      Author: richard
 */

#include "dealer_impl.h"

DealerImpl::DealerImpl() : BJHand(0) {}

const bool DealerImpl::isHitting() const {
    int dealerTotal = getTotal();
    if (isSoft())
        return (dealerTotal < 18);
    else
        return (dealerTotal < 17);
}

std::vector<card> DealerImpl::getFaceUpCards(){
    std::vector<card> faceUpCards;
    for(int i = 0; i < getNumCards(); i++)
    {
        if(getCard(i).isFaceUp()) {
            faceUpCards.push_back(getCard(i));
        }
    }
    return faceUpCards;
}

DealerImpl::~DealerImpl() {
    //std::cout << "BJDealer Destroyed" << std::endl;
}

