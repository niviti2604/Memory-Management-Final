#ifndef STATS_HPP
#define STATS_HPP

#include "memory.hpp"
#include "cache.hpp"  // Add this include

class Stats {
public:
    static void report(const Memory& mem);
    static void reportCache(const Cache& cache);  // New function
    static void reportCombined(const Memory& mem, const Cache& cache);  // New function
};

#endif