/*
 * card.h
 *
 *  Created on: Apr 15, 2018
 *      Author: richard
 */

#ifndef CARD_H_
#define CARD_H_

#include <cstdio>
#include <string>
#include <functional>

class card
{
private:
  int rank;
  int value;
  int HighLowCount;
  int ZenCount;
  int OmmegaIICount;
  bool faceup;
  bool useFlipCallBack;
  std::function<void(card&)> flipCallback;

  void calculateHighLowCount();
  void calculateZenCount();
  void calculateOmegaIICount();

public:
  explicit card(int rank);

  void setFlipCallBack(std::function<void(card&)> setCallback)
  {
    flipCallback = setCallback;
    useFlipCallBack = true;
    if(isFaceUp()){flipCallback(*this);}
  }

  bool operator== (const card &compareCard)
  {
    return rank == compareCard.rank;
  }

  bool operator!= (const card&compareCard)
  {
    return rank != compareCard.rank;
  }

  virtual void flip();
  std::string print() const;

  int getValue() const
  {
    return value;
  }

  int getHighLowCount() const
  {
    return HighLowCount;
  }

  int getZenCount() const
  {
    return ZenCount;
  }

  int getOmegaIICount() const
  {
    return OmmegaIICount;
  }

  bool isFaceUp() const
  {
    return faceup;
  }
  bool isAce() const
  {
    return (rank == 1);
  }
  virtual ~card() = default;

};

#endif /* CARD_H_ */
