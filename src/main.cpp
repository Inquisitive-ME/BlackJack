//
// Created by richard on 7/24/19.
//

#include <iostream>
#include "card.h"
#include "hand.h"
#include "getRandomInt.h"

using namespace std;

int main()
{
  card testCard(11);
  hand testHand(0);

  cout << "Created Card " << testCard.print() << endl;
  testHand.add(testCard);
  cout << "Testing Hand: " << endl;
  testHand.print();

  return 0;
}
