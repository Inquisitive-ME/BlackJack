/*
 * hand.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */
#include "hand.h"

hand::hand(float bet) {
  this->bet = bet;
  total = 0;
  soft = false;
  blackjack = false;
}

void hand::clear()
{
  cards.clear();
}

void hand::add(card cardToAdd)
{
  cards.push_back(cardToAdd);
}

void hand::give(int cardIndex,hand& handToGiveTo)
{
  handToGiveTo.add(cards[cardIndex]);
  cards.erase (cards.begin() + cardIndex);
}

void hand::print()
{
  for(uint i=0; i<cards.size(); i++)
  {
    if(cards[i].isFaceUp())
      std::cout << cards[i].print() << " ";
  }
  std::cout << "Total: " << getTotal() << std::endl;
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
  if (getNumCards() > 0)
  {
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
  }
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