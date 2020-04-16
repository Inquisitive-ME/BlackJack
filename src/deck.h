/*
 * deck.h
 *
 *  Created on: May 24, 2018
 *      Author: richard
 */

#ifndef DECK_H_
#define DECK_H_

#include "hand.h"

#include "player_interface.h"
#include "get_random_int.h"
#include <algorithm>
#include <memory>

class deck : public hand {
private:
    int count;
    uint numberOfDecks;
    uint whenToShuffle;

    void addCountFromCard(card);
    void giveToPlayerHand(uint cardIndex, std::unique_ptr<PlayerInterface>& player, uint playerHandIndex);

public:
    deck();

    deck(uint numDecks, uint whenShuffle);

    void populate();

    int getCount();

    virtual void dealFaceDown(hand &, int);

    virtual void dealFaceUp(hand &, int);

    virtual void dealFaceUp(std::unique_ptr<PlayerInterface>& dealToPlayer, int playerHandNumber);

    void hitPlayerHand(std::unique_ptr<PlayerInterface> &dealToPlayer, int playerHandNumber);

    virtual ~deck();
};

#endif /* DECK_H_ */
