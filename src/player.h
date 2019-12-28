/*
 * player.h
 *
 *  Created on: May 20, 2018
 *      Author: richard
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "BJHand.h"
#include <vector>

class player {
private:
  std::string name;
  float purse;
  std::vector <BJHand> hands;

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
  BJHand& getHand(uint handNum);
  std::vector<BJHand>& getHands();
  virtual void newHand(float);
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
  virtual bool operator==(const player& otherPlayer) const
  {return this == &otherPlayer;};
};

#endif /* PLAYER_H_ */
