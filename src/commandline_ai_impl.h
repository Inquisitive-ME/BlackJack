//
// Created by richard on 4/5/20.
//

#ifndef BLACKJACK_COMMANDLINE_AI_IMPL_H
#define BLACKJACK_COMMANDLINE_AI_IMPL_H

#include "ai_interface.h"
#include <string>

class CommandLineAI : public AiInterface {
public:
    CommandLineAI() = default;

    virtual ~CommandLineAI() = default;

    const int getPlayerBet(const PlayerInterface &gamePlayer) const override;

    const MOVES getMove(const PlayerInterface &gamePlayer) const override;

private:
    int ask_number(std::string question, int low, int high) const;
};

#endif //BLACKJACK_COMMANDLINE_AI_IMPL_H
