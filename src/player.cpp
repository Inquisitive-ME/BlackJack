/*
 * player.cpp
 *
 *  Created on: May 20, 2018
 *      Author: richard
 */

#include "player.h"

#define BLACKJACKMULTIPLIER 1.5
#define SURRENDERPENALTY 0.5

player::player() {
  name = " ";
  purse = 0;
  std::vector <BJHand> hands;
}

// TODO Redo player class so that it is more intelligent
// win should include blackjack
// should have a dealer busted
// isBusted should evaluate automatically

player::player(std::string playerName, float startPurse=0)
{
  name = playerName;
  purse = startPurse;
  std::vector <BJHand> hands;
}

void player::winHand(int handNumber)
{
  if(hands[handNumber].isBlackJack())
  {
    purse += (1.5 * hands[handNumber].getBet());
  }
  else
  {
    purse += hands[handNumber].getBet();
  }
}

void player::loseHand(int handNumber)
{
  purse -= hands[handNumber].getBet();
}

void player::dealerBusted()
{
  for(BJHand playerHand : hands)
  {
    if(playerHand.isBusted())
    {
      purse -= playerHand.getBet();
    }
    else if(playerHand.isBlackJack())
    {
      purse += (playerHand.getBet() * BLACKJACKMULTIPLIER);
    }
    else
    {
      purse += playerHand.getBet();
    }
  }
}

void player::split(int handNum)
{
  BJHand newHand(hands[handNum].getBet());
  hands[handNum].give(0,newHand);
  hands.insert(hands.begin()+handNum,newHand);
  //In the game the additional card is dealt from the deck to each hand
}

void player::doubleDown(int handNum)
{
  hands[handNum].doubleBet();
}

void player::surrender(int handNum)
{
  purse -= (hands[handNum].getBet() * SURRENDERPENALTY);
}

uint player::numHands()
{
  return(hands.size());
}

BJHand& player::getHand(uint handNum)
{
  return(hands[handNum]);
}

std::vector<BJHand>& player::getHands()
{
  return(hands);
}

void player::newHand(float pbet)
{
  hands.push_back(BJHand(pbet));
}

void player::removeHand(int handNumberToRemove)
{
  hands.erase(hands.begin()+handNumberToRemove);
}

void player::clearAllHands()
{
  hands.clear();
}

player::~player() {
//  std::cout << "player Destroyed" << std::endl;
}

