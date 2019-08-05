/*
 * BJDealer.cpp
 *
 *  Created on: May 23, 2018
 *      Author: richard
 */

#include "dealer.h"

dealer::dealer():BJHand(0)
{

}
bool dealer::isHitting()
{
  int dealerTotal=getTotal();
  if(isSoft())
    return (dealerTotal<18);
  else
    return(dealerTotal<17);
}
void dealer::flipFirstCard()
{
  if(getCard(0).isFaceUp())
  {
    getCard(0).flip();
  }
  else
  {
    getCard(0).flip();
  }
}


dealer::~dealer() {
  //std::cout << "BJDealer Destroyed" << std::endl;
}

