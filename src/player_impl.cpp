/*
 * PlayerImpl.cpp
 *
 *  Created on: May 20, 2018
 *      Author: richard
 */

#include "player_impl.h"

#define BLACKJACKMULTIPLIER 1.5
#define SURRENDERPENALTY 0.5

PlayerImpl::PlayerImpl() {
    name = " ";
    purse = 0;
    std::vector<BJHand> hands;
    insurancePayed = false;
}

// TODO Redo PlayerImpl class so that it is more intelligent
// should have a dealer busted
// isBusted should evaluate automatically

PlayerImpl::PlayerImpl(std::string playerName, float startPurse = 0) {
    name = playerName;
    purse = startPurse;
    std::vector<BJHand> hands;
}

void PlayerImpl::winHand(int handNumber) {
    if (hands[handNumber].isBlackJack()) {
        purse += (1.5 * hands[handNumber].getBet());
    } else {
        purse += hands[handNumber].getBet();
    }
}

void PlayerImpl::loseHand(int handNumber) {
    purse -= hands[handNumber].getBet();
}

void PlayerImpl::dealerBusted() {
    for (BJHand playerHand : hands) {
        if (playerHand.isBusted()) {
            purse -= playerHand.getBet();
        } else if (playerHand.isBlackJack()) {
            purse += (playerHand.getBet() * BLACKJACKMULTIPLIER);
        } else {
            purse += playerHand.getBet();
        }
    }
}

void PlayerImpl::split(int handNum) {
    BJHand newHand(hands[handNum].getBet());
    hands[handNum].give(0, newHand);
    hands.insert(hands.begin() + handNum, newHand);
    //In the game the additional card is dealt from the deck to each hand
}

void PlayerImpl::doubleDown(int handNum) {
    hands[handNum].doubleBet();
}

void PlayerImpl::surrender(int handNum) {
    purse -= (hands[handNum].getBet() * SURRENDERPENALTY);
}

void PlayerImpl::payInsurance() {
    insurancePayed = true;
}

const uint PlayerImpl::getNumHands() const {
    return ((uint) hands.size());
}

const BJHand& PlayerImpl::getHand(int handNumber) const {
    return hands[handNumber];
}

void PlayerImpl::moveToHand(int handNumber, card cardToMove){
    hands[handNumber].move_to_hand(cardToMove);
}

void PlayerImpl::hitHand(int handNumber, card cardToAdd){
    moveToHand(handNumber, cardToAdd);
}

void PlayerImpl::newHand(float pbet) {
    hands.emplace_back(BJHand(pbet));
}

void PlayerImpl::removeHand(int handNumberToRemove) {
    hands.erase(hands.begin() + handNumberToRemove);
}

void PlayerImpl::clearAllHands() {
    hands.clear();
}

PlayerImpl::~PlayerImpl() {
//  std::cout << "PlayerImpl Destroyed" << std::endl;
}

