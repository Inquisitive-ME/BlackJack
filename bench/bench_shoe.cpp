// Throughput guard for the deck-history hot path (Phase 1).
// Build is forced to -O2 -march=native regardless of CMAKE_BUILD_TYPE.
#include "sim/shoe.h"

#include <chrono>
#include <cstdio>

int main() {
    bj::Shoe<> shoe(6, 0.75, 12345);
    const long TARGET = 500'000'000; // cards dealt
    long dealt = 0;
    unsigned long long sink = 0; // consume results so nothing is optimized away

    auto t0 = std::chrono::steady_clock::now();
    while (dealt < TARGET) {
        if (shoe.needsShuffle()) shoe.shuffle();
        sink += shoe.dealFaceUp();
        ++dealt;
    }
    auto t1 = std::chrono::steady_clock::now();

    double secs = std::chrono::duration<double>(t1 - t0).count();
    std::printf("dealt %ld cards in %.3f s  ->  %.1f M deals/s  (checksum %llu)\n",
                dealt, secs, dealt / secs / 1e6, sink);
    return 0;
}
