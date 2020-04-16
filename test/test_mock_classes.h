//
// Created by richard on 4/15/20.
//

#ifndef BLACKJACK_TEST_MOCK_CLASSES_H
#define BLACKJACK_TEST_MOCK_CLASSES_H

#include "../src/deck.h"
#include "../src/dealer_interface.h"
#include "../src/player_interface.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class MockDeck : public deck {
public:
    MOCK_METHOD2(dealFaceDown, void(hand&, int));
    MOCK_METHOD2(dealFaceUp, void(PlayerInterface&, int));
};

class MockDealer : public DealerInterface {
public:
    MOCK_CONST_METHOD0(isHitting, const bool());
    MOCK_METHOD0(getFaceUpCards, std::vector<card>());

    MOCK_METHOD1(flipCard, void(int));
    MOCK_METHOD1(getCard, const card(int));
};

class MockPlayer : public PlayerInterface {
public:
    MOCK_METHOD1(winHand, void(int));
    MOCK_METHOD1(loseHand, void(int));
    MOCK_METHOD1(pushHand, void(int));
    MOCK_METHOD0(dealerBusted, void());
    MOCK_METHOD1(split, void(int));
    MOCK_METHOD1(doubleDown, void(int));
    MOCK_METHOD1(surrender, void(int));

    MOCK_CONST_METHOD0(getNumHands, const uint());
    MOCK_CONST_METHOD1(getHand, BJHand &(int));
    MOCK_METHOD2(hitHand, void(int, card));
    MOCK_METHOD2(moveToHand, void(int, card));

    MOCK_METHOD1(removeHand, void(int));
    MOCK_METHOD0(clearAllHands, void());

    MOCK_CONST_METHOD0(getPurse, const float());
    MOCK_CONST_METHOD0(getName, const std::string());
    MOCK_METHOD0(getBet, int());
    MOCK_METHOD1(setBet, void(int bet));
    MOCK_METHOD1(newHand, void(float));
    MOCK_METHOD0(payInsurance, void());
};


#endif //BLACKJACK_TEST_MOCK_CLASSES_H
