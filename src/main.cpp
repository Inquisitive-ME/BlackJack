//
// Created by richard on 7/24/19.
//

#include <iostream>
#include "card.h"
#include "hand.h"
#include "getRandomInt.h"
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
  card testCard(11);
  hand testHand(0);

  cout << "Created Card " << testCard.print() << endl;
  testHand.add(testCard);
  cout << "Testing Hand: " << endl;
  cout << testHand.print() << endl;

  int numNumberToGenerate = 1000000;

  auto start = std::chrono::high_resolution_clock::now();

  int count[10] = {0,0,0,0,0,0,0,0,0,0};
  for(int i = 0; i < numNumberToGenerate; i++)
  {
    int test = getRandomInt(0, 9);
    count[test] = count[test] + 1;
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  cout << "Took " << elapsed.count() << " seconds to generate " << numNumberToGenerate << " random numbers" << endl;

  return 0;
}
