/*
 * player.h
 *
 *  Created on: May 20, 2018
 *      Author: richard
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "hand.h"
#include <vector>

class player {
private:
  std::string name;
  float purse;
  std::vector <hand> hands;

public:
  player();
  player(std::string playerName, float startPurse);
  void winHand(int handNumber);
  void loseHand(int handNumber);
  void dealerBusted();
  void split(int);
  void doubleDown(int);
  void surrender(int);
  uint numHands();
  hand& getHand(uint handNum);
  std::vector<hand>& getHands();
  void newHand(float);
  void removeHand(int handNumberToRemove);
  void clearAllHands();
  float getPurse()
  {
    return(purse);
  }
  std::string getName()
  {
    return(name);
  }
  virtual ~player();
};

#endif /* PLAYER_H_ */
