/*
 * hand.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */
#include "hand.h"
#include <utility>

hand::hand() = default;

void hand::clear() {
    cards.clear();
}

void hand::add(card cardToAdd) {
    cards.push_back(cardToAdd);
}

void hand::move_to_hand(card &cardToAdd) {
    cards.push_back(std::move(cardToAdd));
}

void hand::give(int cardIndex, hand &handToGiveTo) {
    handToGiveTo.move_to_hand(cards[cardIndex]);
    cards.erase(cards.begin() + cardIndex);
}

uint hand::getNumCards() const{
    return (static_cast<uint> (cards.size()));
}

const card &hand::getCard(int cardIndex) const{
    return cards[cardIndex];
};

void hand::flipCard(int cardIndex) {
    cards[cardIndex].flip();
};