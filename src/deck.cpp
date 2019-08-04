/*
 * deck.cpp
 *
 *  Created on: May 24, 2018
 *      Author: richard
 */

#include "deck.h"

deck::deck() {
  count = 0;
  numberOfDecks = 6;
  whenToShuffle = 93;
}

deck::deck(int numDecks, int whenShuffle)
{
  count = 0;
  numberOfDecks = numDecks;
  whenToShuffle = whenShuffle;

}

void deck::populate()
{
  getCards().clear();
  count = 0;
  for(int i=0; i<numberOfDecks; i++)
  {
    for(int j=0; j<4; j++)
    {
      for(int k=1; k<=13; k++)
      {
        card addCard(k);
        add(addCard);
      }
    }
  }
  std::random_shuffle(getCards().begin(),getCards().end());
}

void deck::addCount(int cnt)
{
  count+=cnt;
}

int deck::getCount()
{
  return(count);
}


void deck::deal(hand& dealToHand,int numCards)
{
  for(int i=0; i<numCards; i++)
  {
    if(getNumCards() < whenToShuffle)
    {
      populate();
    }
    // TODO this could be removed for speed if it doesn't add to being more random
    int cardToGive = getRandomInt(0,getNumCards()-1);
    addCount(getCards()[cardToGive].getCount());
    give(cardToGive,dealToHand);
  }
}

void deck::deal(Player& dealToPlayer,int numCards)
{
  for(hand pHand : dealToPlayer.getHand())
  {
    for(int j=0; j<numCards; j++)
    {
      if(getNumCards() < whenToShuffle)
        populate();
      int cardToGive = getRandomInt(0,getNumCards()-1);
      addCount(getCards()[cardToGive].getCount());
      give(cardToGive, phand);
    }
  }
}

deck::~deck() {
  std::cout << "deck Destroyed" << std::endl;
}

