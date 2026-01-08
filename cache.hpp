#ifndef CACHE_HPP
#define CACHE_HPP
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <cstdint>

struct CacheLine {
    uint64_t tag;
    bool valid;
    bool dirty;
    int64_t timestamp;  // For LRU/FIFO
    int freq;           // For LFU
    
    // FIX: Initialize members in correct order (same as declaration)
    CacheLine() : tag(0), valid(false), dirty(false), timestamp(0), freq(0) {}
};

class Cache {
public:
    Cache(size_t size, size_t block_size, int associativity, std::string policy);
    bool access(uint64_t address, bool& hit);
    void report() const;
        size_t getBlockSize() const { return block_size; }
    // GETTER METHODS - KEEP ONLY ONE COPY!
    int getHits() const { return hits; }
    int getMisses() const { return misses; }
    double getHitRate() const {
        int total_accesses = hits + misses;
        return (total_accesses > 0) ? (double)hits / total_accesses * 100.0 : 0.0;
    }
    
private:
    size_t size, block_size, num_sets;
    int associativity;
    std::string policy;
    
    std::vector<std::vector<CacheLine>> sets_data;
    std::vector<std::list<int>> lru_order;    // One list per set for LRU order
    std::vector<std::queue<int>> fifo_order;  // One queue per set for FIFO order
    
    int hits = 0, misses = 0;
    int64_t current_time = 0;
    
    // Helper methods
    size_t getSetIndex(uint64_t address) const;
    uint64_t getTag(uint64_t address) const;
    size_t getBlockOffset(uint64_t address) const;
    
    // Replacement policies
    int findLRUVictim(int set_index);
    int findLFUVictim(int set_index);
    int findFIFOVictim(int set_index);
    void updateAccess(int set_index, int way, bool is_hit);
};

#endif