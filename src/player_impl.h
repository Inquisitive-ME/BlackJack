/*
 * PlayerImpl.h
 *
 *  Created on: May 20, 2018
 *      Author: richard
 */

#ifndef PLAYER_IMPL_H_
#define PLAYER_IMPL_H_

#include "player_interface.h"
#include "player_impl.h"

class PlayerImpl : public  PlayerInterface{
private:
  std::string name;
  float purse;
  std::vector <BJHand> hands;

public:
  PlayerImpl();
  PlayerImpl(std::string playerName, float startPurse);

  void winHand(int handNumber) override;
  void loseHand(int handNumber) override;
  void dealerBusted() override;
  void split(int) override;
  void doubleDown(int) override;
  void surrender(int) override;
  const uint numHands() const override;
  BJHand& getHand(uint handNum) override;
  std::vector<BJHand>& getHands() override;
  virtual void newHand(float) override;
  void removeHand(int handNumberToRemove) override;
  void clearAllHands() override;
  const float getPurse() const override
  {
    return(purse);
  }
  const std::string getName() const override
  {
    return(name);
  }
  virtual ~PlayerImpl();
  virtual bool operator==(const PlayerImpl& otherPlayer) const
  {return this == &otherPlayer;};
};

#endif /* PLAYER_H_ */
