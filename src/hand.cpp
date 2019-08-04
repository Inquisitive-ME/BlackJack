/*
 * hand.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */
#include "hand.h"
#include <utility>

hand::hand(float bet = 0) {
  this->bet = bet;
  total = 0;
  soft = false;
}

void hand::clear()
{
  cards.clear();
}

void hand::add(card cardToAdd)
{
  cards.push_back(cardToAdd);
}

void hand::add_move(card cardToAdd)
{
  cards.push_back(std::move(cardToAdd));
}
void hand::give(int cardIndex,hand& handToGiveTo)
{
  handToGiveTo.add_move(cards[cardIndex]);
  cards.erase (cards.begin() + cardIndex);
}

std::string hand::print()
{
  std::string returnString = "";
  for(uint i=0; i<cards.size(); i++)
  {
    if(cards[i].isFaceUp())
      returnString.append(cards[i].print());
  }
  returnString.append(" Total: ");
  returnString.append(std::to_string(getTotal()));

  return returnString;
}

uint hand::getNumCards()
{
  return(cards.size());
}

std::vector<card>& hand::getCards()
{
  return cards;
}

int hand::getTotal()
{
  total =0;
  bool hasAce = false;
  for(uint i=0; i < getNumCards(); i++)
  {
    if(getCards()[i].isFaceUp())
    {
      if(getCards()[i].isAce())
      {
        hasAce = true;
      }
      total+= getCards()[i].getValue();
    }
  }
  if(hasAce && total <= 11)
  {
    soft = true;
    total +=10;
  }
  else
    soft = false;
  return total;
}

bool hand::isBusted()
{
  if(getTotal() > 21)
  {
    return(true);
  }
  else
    return false;
}

bool hand::isBlackJack()
{
  return (getTotal() == 21 && cards.size() == 2);
}