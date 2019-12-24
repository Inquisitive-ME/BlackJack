//
// Created by richard on 12/23/19.
//

#ifndef BLACKJACK_ABSTRACTGAMEPLAYER_H
#define BLACKJACK_ABSTRACTGAMEPLAYER_H

#include <string>
#include <list>

#include "player.h"

enum MOVES {HIT, STAY, SPLIT, DOUBLED, SURRENDER};

class abstractGamePlayer: public player {
public:
    abstractGamePlayer(){}
    abstractGamePlayer(std::string playerName, float startPurse=0) : player(playerName, startPurse) {};
    virtual ~abstractGamePlayer(){}

    virtual int getBet() = 0;
    //virtual bool purchaseInsurance(int count);
    virtual MOVES getMove() = 0;

};

#endif //BLACKJACK_ABSTRACTGAMEPLAYER_H
