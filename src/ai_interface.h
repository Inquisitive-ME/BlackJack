//
// Created by richard on 12/23/19.
//

#ifndef BLACKJACK_ABSTRACTGAMEPLAYER_H
#define BLACKJACK_ABSTRACTGAMEPLAYER_H

#include "player_interface.h"

class AiInterface {
public:
    virtual ~AiInterface() = default;

    virtual const int getPlayerBet(const std::unique_ptr<PlayerInterface>& gamePlayer) = 0;

    virtual const MOVES getMove(std::vector<card> dealerHand, const BJHand& playerHand) = 0;

    virtual const bool continuePlaying() = 0;

    virtual const bool payInsurance(const PlayerInterface &gamePlayer) = 0;

    virtual bool illegalMove() = 0;

};

#endif //BLACKJACK_ABSTRACTGAMEPLAYER_H
