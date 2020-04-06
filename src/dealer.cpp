/*
 * BJDealer.cpp
 *
 *  Created on: May 23, 2018
 *      Author: richard
 */

#include "dealer.h"

dealer::dealer() : BJHand(0) {
}

bool dealer::isHitting() {
    int dealerTotal = getTotal();
    if (isSoft())
        return (dealerTotal < 18);
    else
        return (dealerTotal < 17);
}

std::vector<card> dealer::getFaceUpCards(){
    std::vector<card> faceUpCards;
    for(int i = 0; i < getNumCards(); i++)
    {
        if(getCard(i).isFaceUp()) {
            faceUpCards.push_back(getCard(i));
        }
    }
    return faceUpCards;
}

dealer::~dealer() {
    //std::cout << "BJDealer Destroyed" << std::endl;
}

