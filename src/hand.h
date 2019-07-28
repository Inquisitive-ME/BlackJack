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
  bool blackjack;

public:
  hand(float bet);

  void clear();
  void print();
  void add(card cardToAdd);
  void give(int cardIndex, hand& handToGiveTo);
  uint getNumCards();
  std::vector<card>& getCards();
  int getTotal();
  bool isBusted();

  float getBet()
  {
    return bet;
  }

  bool isSoft()
  {
    return soft;
  }

  void doubleDown()
  {
    bet *= 2;
  }

  void blackJack()
  {
    bet *= 1.5;
    blackjack = true;
  }

  void Surrender()
  {
    bet *= 0.5;
  }

  bool hasBlackJack()
  {
    return blackjack;
  }

  virtual ~hand()
  {
    //std::cout << "Hand Destroyed" << std::endl;
  }
};



#endif /* HAND_H_ */