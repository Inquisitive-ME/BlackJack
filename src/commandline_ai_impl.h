//
// Created by richard on 4/5/20.
//

#ifndef BLACKJACK_COMMANDLINE_AI_IMPL_H
#define BLACKJACK_COMMANDLINE_AI_IMPL_H

#include "ai_interface.h"

class CommandLineAI : public AiInterface{
public:
  CommandLineAI() = default;
  virtual ~CommandLineAI() = default;

  const int getPlayerBet(const PlayerInterface& gamePlayer) override;
  const MOVES getMove(const PlayerInterface& gamePlayer) override;




  int getPlayerBet(const PlayerInterface& gamePlayer){
    string question = gamePlayer.getName() + " how much would you like to bet?";
    return int(ask_number(question, 5, 500));
  }

private:
  int ask_number(std::string question, int low,int high);
};
#endif //BLACKJACK_COMMANDLINE_AI_IMPL_H
