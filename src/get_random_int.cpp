/*
 * getRandomInt.cpp
 *
 *  Created on: May 5, 2018
 *      Author: richard
 */

#include "get_random_int.h"
#include <random>

namespace {
    // One engine per thread, seeded once from a nondeterministic source on
    // first use. Override with seedRandom() for reproducible sequences.
    std::mt19937_64 &engine() {
        static thread_local std::mt19937_64 eng{std::random_device{}()};
        return eng;
    }
}

void seedRandom(uint64_t seed) {
    engine().seed(seed);
}

int getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine());
}
