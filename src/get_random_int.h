/*
 * getRandomInt.h
 *
 *  Created on: May 5, 2018
 *      Author: richard
 */

#ifndef GETRANDOMINT_H_
#define GETRANDOMINT_H_

#include <cstdint>

// Uniformly-distributed integer in [min, max], drawn from a single shared
// engine that is seeded once (lazily) on first use. This replaces the old
// per-call std::random_device + mt19937_64 reseed, which was ~400x slower.
int getRandomInt(int min, int max);

// Reseed the shared engine so a run is reproducible (RL rollouts, tests).
// Note: the engine is thread_local, so this seeds the calling thread's engine.
void seedRandom(uint64_t seed);

#endif /* GETRANDOMINT_H_ */
