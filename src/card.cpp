/*
 * card.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */
#include "card.h"

card::card()
{
  rank = 0;
  value =0;
  faceup=false;
  count = 0;

}
card::card(int rank)
{
  this->rank = rank;

  if(rank >= 1 && rank <= 10)
    value = rank;
  else if(rank > 10)
    value = 10;
  else
    printf("Using Invalid Ranks");

  faceup = true;

  if(faceup)
  {
    switch(COUNT_TYPE){
      //High Low
      case 0:
      {
        if((rank >= 2) && (rank <= 6))
          count = 1;
        else if(rank >=10 || rank == 1)
          count = -1;
        else
          count = 0;
      }
      break;
      //Zen
      case 1:
      {
        if(rank == 2 || rank ==3 || rank == 7)
          count = 1;
        else if(rank == 4 || rank ==5 || rank == 6)
          count = 2;
        else if(rank == 1)
          count = -1;
        else if(rank >=10)
          count = -2;
        else
          count = 0;
      }
      break;
      //OmegaII
      case 2:
      {
        if(rank == 2 || rank ==3 || rank == 7)
          count = 1;
        else if(rank == 4 || rank ==5 || rank == 6)
          count = 2;
        else if(rank == 9)
          count = -1;
        else if(rank >=10)
          count = -2;
        else
          count = 0;
      }
      break;
    }
  }
  else
    count =0;
}

void card::flip()
{
  faceup = !faceup;
  if(faceup)
  {
    switch(COUNT_TYPE){
      //High Low
      case 0:
      {
        if(rank >= 2 && rank <= 6)
          count = 1;
        else if(rank >=10)
          count = -1;
        else
          count = 0;
      }
      break;
      //Zen
      case 1:
      {
        if(rank == 2 || rank ==3 || rank == 7)
          count = 1;
        else if(rank == 4 || rank ==5 || rank == 6)
          count = 2;
        else if(rank == 1)
          count = -1;
        else if(rank >=10)
          count = -2;
        else
          count = 0;
      }
      break;
      //OmegaII
      case 2:
      {
        if(rank == 2 || rank ==3 || rank == 7)
          count = 1;
        else if(rank == 4 || rank ==5 || rank == 6)
          count = 2;
        else if(rank == 9)
          count = -1;
        else if(rank >=10)
          count = -2;
        else
          count = 0;
      }
      break;
    }
  }
  else
    count =0;
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


