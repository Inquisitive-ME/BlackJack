/*
 * deck.cpp
 *
 *  Created on: May 24, 2018
 *      Author: richard
 */

#include "deck.h"

deck::deck() {
    count = 0;
    numberOfDecks = 6;
    whenToShuffle = 93;
}

deck::deck(int numDecks, int whenShuffle) {
    count = 0;
    numberOfDecks = numDecks;
    whenToShuffle = whenShuffle;
}

void deck::populate() {
    clear();
    count = 0;
    for (uint i = 0; i < numberOfDecks; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 1; k <= 13; k++) {
                card addCard(k);
                addCard.setFlipCallBack(std::bind(&deck::addCountFromCard, this, std::placeholders::_1));
                add(addCard);
            }
        }
    }
    std::random_shuffle(cards.begin(), cards.end());
}

void deck::addCountFromCard(card cardToAddCount) {
    if (cardToAddCount.isFaceUp()) {
        count += cardToAddCount.getHighLowCount();
    } else {
        count -= cardToAddCount.getHighLowCount();
    }
}

int deck::getCount() {
    return (count);
}


void deck::deal(hand &dealToHand, int numCards) {
    for (int i = 0; i < numCards; i++) {
        // TODO this could be removed for speed if it doesn't add to being more random
        int cardToGive = getRandomInt(0, getNumCards() - 1);
        give(cardToGive, dealToHand);
        if (getNumCards() <= whenToShuffle) {
            populate();
        }
    }
}

void deck::deal(PlayerInterface &dealToPlayer, int numCards) {
    for (BJHand &pHand : dealToPlayer.getHands()) {
        for (int j = 0; j < numCards; j++) {
            int cardToGive = getRandomInt(0, getNumCards() - 1);
            give(cardToGive, pHand);
            if (getNumCards() <= whenToShuffle) {
                populate();
            }
        }
    }
}

deck::~deck() {
    std::cout << "deck Destroyed" << std::endl;
}

