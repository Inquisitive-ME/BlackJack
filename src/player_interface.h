//
// Created by richard on 4/5/20.
//

#ifndef BLACKJACK_PLAYER_INTERFACE_H
#define BLACKJACK_PLAYER_INTERFACE_H

#include "BJHand.h"
#include <vector>

enum MOVES {HIT, STAY, SPLIT, DOUBLED, SURRENDER};

class PlayerInterface {
public:
  virtual ~PlayerInterface() = default;

  virtual void winHand(int handNumber) = 0;
  virtual void loseHand(int handNumber) = 0;
  virtual void dealerBusted() = 0;
  virtual void split(int) = 0;
  virtual void doubleDown(int) = 0;
  virtual void surrender(int) = 0;
  virtual const uint numHands() const = 0;
  virtual BJHand& getHand(uint handNum) = 0;
  virtual std::vector<BJHand>& getHands() = 0;
  virtual void newHand(float) = 0;
  virtual  void removeHand(int handNumberToRemove) = 0;
  virtual  void clearAllHands() = 0;
  virtual const float getPurse() const = 0;
  virtual const std::string getName() const = 0;
  virtual bool operator==(const PlayerInterface& otherPlayer) const
  {return this == &otherPlayer;};
};

#endif //BLACKJACK_PLAYER_INTERFACE_H
