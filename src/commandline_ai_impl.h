//
// Created by richard on 4/5/20.
//

#ifndef BLACKJACK_COMMANDLINE_AI_IMPL_H
#define BLACKJACK_COMMANDLINE_AI_IMPL_H

#include "ai_interface.h"
#include <string>

class CommandLineAI : public AiInterface {
public:
    CommandLineAI();

    virtual ~CommandLineAI() = default;

    const int getPlayerBet(const PlayerInterface &gamePlayer) override;

    const MOVES getMove(std::vector<card> dealerHand, const BJHand &playerHand) override;

    const bool continuePlaying() override;

    const bool payInsurance(const PlayerInterface &gamePlayer) override;

    bool illegalMove();

private:
    int rounds;
    MOVES lastMove;
    int ask_number(std::string question, int low, int high) const;
    MOVES ask_move();
};

#endif //BLACKJACK_COMMANDLINE_AI_IMPL_H
