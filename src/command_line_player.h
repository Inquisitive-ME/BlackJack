//
// Created by richard on 4/12/20.
//

#ifndef BLACKJACK_COMMAND_LINE_PLAYER_H
#define BLACKJACK_COMMAND_LINE_PLAYER_H

#include "player_interface.h"
#include "player_impl.h"

class CommandLinePlayer : public PlayerImpl{

public:
    CommandLinePlayer();

    CommandLinePlayer(std::string playerName, float startPurse);

    void winHand(int handNumber) override;

    void loseHand(int handNumber) override;

    void pushHand(int handNumber) override;

    void dealerBusted() override;

    virtual ~CommandLinePlayer();
};


#endif //BLACKJACK_COMMAND_LINE_PLAYER_H
