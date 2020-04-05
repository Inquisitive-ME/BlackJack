//
// Created by richard on 4/5/20.
//

#include "commandline_ai_impl.h"
#include <iostream>
#include <string>

const int CommandLineAI::getPlayerBet(const PlayerInterface &gamePlayer)
{
  int min_bet = 5;
  int max_bet = 500;
  std::string question = gamePlayer.getName() + " how much would you like to bet? table min $" + std::to_string(min_bet) + " table max $" + std::to_string(max_bet);
  return ask_number("How much would you like to bet? table min $%d table max $%d", min_bet, max_bet)
}

int CommandLineAI::ask_number(std::string question, int low, int high)
{
  {
    int response = low - 1;
    cout << question << endl;
    cin >> response;

    if(response >= low && response <=high)
      return response;
    else
    {
      cout << "Please enter a number between " << low << " and " << high << endl;
      cin.clear();
      std::string ignoreLine; //read the invalid input into it
      std::getline(cin, ignoreLine); //read the line till next space
      return ask_number(question,low,high);
    }
  }
}

const MOVES CommandLineAI::getMove(const PlayerInterface &gamePlayer)
{
  return STAY;
}

