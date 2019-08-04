/*
 * card.h
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */

#ifndef CARD_H_
#define CARD_H_

#include <stdio.h>
#include <string>

//Count can be 0 = High Low, 1 = Zen, 2 = OmegaII
#define COUNT_TYPE 0

class card
{
private:
  int rank;
  int value;
  int HighLowCount;
  int ZenCount;
  int OmmegaIICount;
  bool faceup;

  void calculateHighLowCount();
  void calculateZenCount();
  void calculateOmegaIICount();

public:
  card(int rank);

  bool operator== (const card &compareCard)
  {
    return rank == compareCard.rank;
  }

  bool operator!= (const card&compareCard)
  {
    return rank != compareCard.rank;
  }

  void flip();
  std::string print();

  int getValue()
  {
    return value;
  }

  int getHighLowCount()
  {
    return HighLowCount;
  }

  int getZenCount()
  {
    return ZenCount;
  }

  int getOmegaIICount()
  {
    return OmmegaIICount;
  }

  bool isFaceUp()
  {
    return faceup;
  }
  bool isAce()
  {
    return (rank == 1);
  }
  ~card()
  {
    //printf("Card Destroyed\n");
  };

};

#endif /* CARD_H_ */
