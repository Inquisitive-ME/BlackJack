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

deck::deck(uint numDecks, uint whenShuffle) {
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
                copy_to_hand(addCard);
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


void deck::dealFaceDown(hand &dealToHand, int numCards) {
    for (int i = 0; i < numCards; i++) {
        // TODO this could be removed for speed if it doesn't add to being more random
        int cardToGive = getRandomInt(0, getNumCards() - 1);
        give(cardToGive, dealToHand);
        if (getNumCards() <= whenToShuffle) {
            populate();
        }
    }
}

void deck::dealFaceUp(hand &dealToHand, int numCards) {
    for (int i = 0; i < numCards; i++) {
        // TODO this could be removed for speed if it doesn't add to being more random
        int cardToGive = getRandomInt(0, getNumCards() - 1);
        cards[cardToGive].flip();
        give(cardToGive, dealToHand);
        if (getNumCards() <= whenToShuffle) {
            populate();
        }
    }
}

void deck::giveToPlayerHand(uint cardIndex, std::unique_ptr<PlayerInterface>& player, uint playerHandIndex){
    player->moveToHand(playerHandIndex, cards[cardIndex]);
    cards.erase(cards.begin() + cardIndex);
}

void deck::dealFaceUp(std::unique_ptr<PlayerInterface>& dealToPlayer, int numCards) {
    for (uint handNumber = 0; handNumber < dealToPlayer->getNumHands(); handNumber++) {
        for (int j = 0; j < numCards; j++) {
            uint cardToGive = (uint) getRandomInt(0, getNumCards() - 1);
            cards[cardToGive].flip();
            giveToPlayerHand(cardToGive, dealToPlayer, handNumber);
            if (getNumCards() <= whenToShuffle) {
                populate();
            }
        }
    }
}

void deck::hitPlayerHand(std::unique_ptr<PlayerInterface>& dealToPlayer, int playerHandNumber){
    uint cardToGive = (uint) getRandomInt(0, getNumCards() - 1);
    cards[cardToGive].flip();
    giveToPlayerHand(cardToGive, dealToPlayer, playerHandNumber);
    if (getNumCards() <= whenToShuffle) {
        populate();
    }
}

deck::~deck() {
    std::cout << "deck Destroyed" << std::endl;
}

