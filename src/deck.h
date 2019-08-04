/*
 * deck.h
 *
 *  Created on: May 24, 2018
 *      Author: richard
 */

#ifndef DECK_H_
#define DECK_H_

#include "hand.h"

#include "player.h"
#include "getRandomInt.h"
#include <algorithm>

class deck: public hand {
private:
  int count;
  uint numberOfDecks;
  uint whenToShuffle;
public:
  deck();
  deck(int numDecks,int whenShuffle);
  void populate();
  int getCount();
  void addCount(int);
  void deal(hand&,int);
  void deal(Player&,int);
  virtual ~deck();
};

#endif /* DECK_H_ */
