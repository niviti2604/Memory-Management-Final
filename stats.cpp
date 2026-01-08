#include "stats.hpp"
#include <iostream>
#include <algorithm>

void Stats::report(const Memory& mem) {
    std::cout << std::dec;

    size_t total_memory = mem.getTotalSize();
    size_t used_memory = mem.getUsedSize();

    size_t total_free = 0;
    size_t largest_free = 0;
    size_t internal_fragmentation = 0;

    const Block* curr = mem.getHead();

    while (curr) {
        if (curr->free) {
            total_free += curr->size;
            largest_free = std::max(largest_free, curr->size);
        } else {
            internal_fragmentation += (curr->size - curr->requested_size);
        }
        curr = curr->next;
    }

    double external_fragmentation =
        (total_free == 0) ? 0.0 :
        (1.0 - (static_cast<double>(largest_free) / total_free)) * 100.0;
      
    double memory_utilization =
        (double)used_memory / total_memory * 100.0;

    std::cout << "\n===== Memory Statistics =====\n";
    std::cout << "Total memory: " << total_memory << "\n";
    std::cout << "Used memory: " << used_memory << "\n";
    std::cout << "Free memory: " << total_free << "\n";
    std::cout << "Memory utilization: " << memory_utilization << "%\n";
    std::cout << "Internal fragmentation: " << internal_fragmentation << " bytes\n";
    std::cout << "External fragmentation: " << external_fragmentation << "%\n";
    std::cout << "==============================\n";
}

void Stats::reportCache(const Cache& cache) {
    int hits = cache.getHits();
    int misses = cache.getMisses();
    int total_accesses = hits + misses;
    double hit_rate = cache.getHitRate();
    
    std::cout << "\n===== Cache Statistics =====\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";
    std::cout << "Total accesses: " << total_accesses << "\n";
    std::cout << "Hit rate: " << hit_rate << "%\n";
    std::cout << "Miss rate: " << (100.0 - hit_rate) << "%\n";
    std::cout << "=============================\n";
}

void Stats::reportCombined(const Memory& mem, const Cache& cache) {
    report(mem);
    reportCache(cache);
}