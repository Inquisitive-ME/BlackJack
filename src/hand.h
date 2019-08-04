/*
 * hand.h
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */

#ifndef HAND_H_
#define HAND_H_

#include <iostream>
#include <vector>
#include "card.h"
class hand
{
private:
  void add_move(card cardToMove);

protected:
  std::vector<card> cards;

public:
  hand();

  void clear();
  void add(card cardToAdd);
  void give(int cardIndex, hand& handToGiveTo);
  uint getNumCards();
  card getCard(int cardIndex);

  virtual ~hand()
  {
    //std::cout << "Hand Destroyed" << std::endl;
  }
};



#endif /* HAND_H_ */