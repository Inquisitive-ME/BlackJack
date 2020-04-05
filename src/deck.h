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

class deck : public hand {
private:
    int count;
    uint numberOfDecks;
    uint whenToShuffle;

    void addCountFromCard(card);

public:
    deck();

    deck(int numDecks, int whenShuffle);

    void populate();

    int getCount();

    virtual void deal(hand &, int);

    virtual void deal(PlayerInterface &, int);

    virtual ~deck();
};

#endif /* DECK_H_ */
