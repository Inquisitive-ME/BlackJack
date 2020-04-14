//
// Created by richard on 4/12/20.
//

#include "command_line_player.h"

// TODO implement same function in command line dealer to display when card is added

CommandLinePlayer::CommandLinePlayer() : PlayerImpl() {}

CommandLinePlayer::CommandLinePlayer(std::string playerName, float startPurse = 0) : PlayerImpl(playerName, startPurse) {}

void CommandLinePlayer::winHand(int handNumber) {
    PlayerImpl::winHand(handNumber);
    std::cout << getName() << " won $" << getHand(handNumber).getBet() << " and now has $" << getPurse() << std::endl;
}

void CommandLinePlayer::loseHand(int handNumber) {
    PlayerImpl::loseHand(handNumber);
    if(PlayerImpl::getHand(handNumber).isBusted()){
        std::cout << PlayerImpl::getName() << " busted and";
    } else{
        std::cout << getName();
    }
    std::cout << " lost $" << getHand(handNumber).getBet() <<  " and now has $" << getPurse() << std::endl;
}

void CommandLinePlayer::pushHand(int handNumber) {
    PlayerImpl::loseHand(handNumber);
    std::cout << getName() << " pushed" << " you have $" << getPurse() << std::endl;
}

void CommandLinePlayer::dealerBusted() {
    PlayerImpl::dealerBusted();
    std::cout << "Dealer Busted" << std::endl;
}

CommandLinePlayer::~CommandLinePlayer() {}
