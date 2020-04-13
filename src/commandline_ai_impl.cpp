//
// Created by richard on 4/5/20.
//

#include "commandline_ai_impl.h"
#include <iostream>
#include <string>

CommandLineAI::CommandLineAI() {
    rounds = 0;
}

const int CommandLineAI::getPlayerBet(const PlayerInterface &gamePlayer) {
    int min_bet = 5;
    int max_bet = 500;
    std::string question = gamePlayer.getName() + " you have $" + std::to_string(gamePlayer.getPurse()) +
                           " how much would you like to bet? table min $" +
                           std::to_string(min_bet) +
                           " table max $" + std::to_string(max_bet);
    return ask_number(question, min_bet, max_bet);
}


const MOVES CommandLineAI::getMove(std::vector<card> dealerHand, const BJHand &playerHand) {
    std::cout << "Dealer Hand: ";
    for (card dealerCard : dealerHand) {
        std::cout << dealerCard.print() << " ";
    };
    std::cout << std::endl;

    std::cout << "For hand " << playerHand.print() << " ";

    lastMove = ask_move();
    return lastMove;
}

const bool CommandLineAI::continuePlaying() {
    std::cout << "Continue if round is less than 2. Round: " << rounds << std::endl;
    rounds++;
    return rounds <= 2;
}

const bool CommandLineAI::payInsurance(const PlayerInterface &gamePlayer) {
    std::cout << "NO Insurance" << std::endl;
    return false;
}

bool CommandLineAI::illegalMove(){

    std::cout << MOVE_TO_STRING[lastMove] << std::endl;
    return true;
}

int CommandLineAI::ask_number(std::string question, int low, int high) const {
    {
        int response = low - 1;
        std::cout << question << std::endl;
        std::cin >> response;

        if (response >= low && response <= high)
            return response;
        else {
            std::cout << "Please enter a number between " << low << " and " << high << std::endl;
            std::cin.clear();
            std::string ignoreLine; //read the invalid input into it
            std::getline(std::cin, ignoreLine); //read the line till next space
            return ask_number(question, low, high);
        }
    }
}

MOVES CommandLineAI::ask_move() {
    std::string response = "";
    std::cout << "What would you like to do?" << std::endl;
    std::cin >> response;

    if (response == "split")
        return SPLIT;
    else if (response == "hit")
        return HIT;
    else if (response == "stay")
        return STAY;
    else if (response == "surrender")
        return SURRENDER;
    else if (response == "double")
        return DOUBLED;
    else {
        std::cout << "Please enter \"split\",\"hit\",\"stay\",\"surrender\", or \"double\"" << std::endl;
        return ask_move();
    }
}

