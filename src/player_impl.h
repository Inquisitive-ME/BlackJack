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

class PlayerImpl : public PlayerInterface {
private:
    std::string name;
    float purse;
    std::vector<BJHand> hands;
    bool insurancePayed;

public:
    PlayerImpl();

    PlayerImpl(std::string playerName, float startPurse);

    virtual void winHand(int handNumber) override;

    virtual void loseHand(int handNumber) override;

    virtual void pushHand(int handNumber) override {};

    virtual void dealerBusted() override;

    void split(int handNumber) override;

    void doubleDown(int handNumber) override;

    void surrender(int handNumber) override;

    void payInsurance() override;

    const uint getNumHands() const override;

    const BJHand& getHand(int handNumber) const override;

    virtual void moveToHand(int handNumber, card cardToMove) override;
    virtual void hitHand(int handNumber, card cardToMove) override;

    virtual void newHand(float) override;

    void removeHand(int handNumberToRemove) override;

    void clearAllHands() override;

    const float getPurse() const override {
        return (purse);
    }

    const std::string getName() const override {
        return (name);
    }

    virtual ~PlayerImpl();
};

#endif /* PLAYER_H_ */
