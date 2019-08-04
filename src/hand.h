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
  std::vector<card> cards;
  float bet;
  int total;
  bool soft;
  void add_move(card cardToMove);

public:
  hand(float bet);

  void clear();
  std::string print();
  void add(card cardToAdd);
  void give(int cardIndex, hand& handToGiveTo);
  uint getNumCards();
  std::vector<card>& getCards();
  int getTotal();
  bool isBusted();
  bool isBlackJack();

  float getBet()
  {
    return bet;
  }

  void doubleBet()
  {
    bet *= 2;
  }

  bool isSoft()
  {
    return soft;
  }

  virtual ~hand()
  {
    //std::cout << "Hand Destroyed" << std::endl;
  }
};



#endif /* HAND_H_ */