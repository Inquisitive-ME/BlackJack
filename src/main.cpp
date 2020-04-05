//
// Created by richard on 7/24/19.
//

#include <iostream>
#include "card.h"
#include "bj_hand.h"
#include "player_impl.h"
#include "get_random_int.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
    card testCard(11);
    BJHand testHand(0);
    PlayerImpl testPlayer;

    cout << "Created Card " << testCard.print() << endl;
    testHand.add(testCard);
    cout << "Test Hand: " << testHand.print() << endl;
    testPlayer.getHands().push_back(testHand);
    cout << "test Players Hand = " << testPlayer.getHands()[0].print() << endl;

    int numNumberToGenerate = 1000000;

    auto start = std::chrono::high_resolution_clock::now();

    int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < numNumberToGenerate; i++) {
        int test = getRandomInt(0, 9);
        count[test] = count[test] + 1;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    cout << "Took " << elapsed.count() << " seconds to generate " << numNumberToGenerate << " random numbers" << endl;

    return 0;
}
