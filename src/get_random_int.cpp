/*
 * getRandomInt.cpp
 *
 *  Created on: May 5, 2018
 *      Author: richard
 */

#include "get_random_int.h"

int getRandomInt(int min, int max) {
    double rnum = 0;
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_real_distribution<double> dist(min, max + 1);

    rnum = dist(mt);

    return (int) (rnum);
}



