/*
 * card.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */
#include "card.h"
#include <iostream>
card::card(int rank)
{
  this->rank = rank;

  if(rank >= 1 && rank <= 10)
    value = rank;
  else if(rank > 10)
    value = 10;
  else
    printf("Using Invalid Ranks");

  //TODO try starting with card faceup so that we don't have to do a flip and callback for most of the cards dealt
  faceup = false;
  calculateHighLowCount();
  calculateZenCount();
  calculateOmegaIICount();

  useFlipCallBack = false;
}

void card::flip()
{
  faceup = !faceup;
  if(useFlipCallBack){flipCallback(*this);}
}

void card::calculateHighLowCount()
{
  if((rank >= 2) && (rank <= 6))
    HighLowCount = 1;
  else if(rank >=10 || rank == 1)
    HighLowCount = -1;
  else
    HighLowCount = 0;
}

void card::calculateZenCount()
{
  if(rank == 2 || rank ==3 || rank == 7)
    ZenCount = 1;
  else if(rank == 4 || rank ==5 || rank == 6)
    ZenCount = 2;
  else if(rank == 1)
    ZenCount = -1;
  else if(rank >=10)
    ZenCount = -2;
  else
    ZenCount = 0;
}

void card::calculateOmegaIICount()
{
  if(rank == 2 || rank ==3 || rank == 7)
    OmmegaIICount = 1;
  else if(rank == 4 || rank ==5 || rank == 6)
    OmmegaIICount = 2;
  else if(rank == 9)
    OmmegaIICount = -1;
  else if(rank >=10)
    OmmegaIICount = -2;
  else
    OmmegaIICount = 0;
}

std::string card::print()
{
  if(rank >=2 && rank <=10)
    return std::to_string(rank);
  else if(rank ==1)
    return "A";
  else if(rank == 11)
    return "J";
  else if(rank ==12)
    return "Q";
  else if(rank == 13)
    return "K";
  else
  {
    printf("Invalid Rank in Print");
    return "0";
  }
}