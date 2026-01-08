#include "cache.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Cache::Cache(size_t size, size_t block_size, int associativity, std::string policy)
    : size(size),
      block_size(block_size),
      associativity(associativity),
      policy(policy),
      hits(0),
      misses(0),
      current_time(0)
{
    // Calculate number of sets
    num_sets = size / (block_size * associativity);
    sets_data.resize(num_sets, std::vector<CacheLine>(associativity));
    
    // Initialize LRU/FIFO structures if needed
    if (policy == "LRU") {
        lru_order.resize(num_sets);
        for (size_t i = 0; i < num_sets; i++) {  // Changed int to size_t
            for (int j = 0; j < associativity; j++) {
                lru_order[i].push_back(j);
            }
        }
    } else if (policy == "FIFO") {
        fifo_order.resize(num_sets);
        for (size_t i = 0; i < num_sets; i++) {  // Changed int to size_t
            for (int j = 0; j < associativity; j++) {
                fifo_order[i].push(j);
            }
        }
    }
    
    std::cout << "Cache initialized: " 
              << num_sets << " sets, " 
              << associativity << "-way, "
              << "block size: " << block_size 
              << " bytes, Policy: " << policy << std::endl;
}

// Calculate set index from address
size_t Cache::getSetIndex(uint64_t address) const {
    uint64_t block_address = address / block_size;
    return block_address % num_sets;
}

// Calculate tag from address
uint64_t Cache::getTag(uint64_t address) const {
    uint64_t block_address = address / block_size;
    return block_address / num_sets;  // Remove set index bits
}

// Calculate block offset
size_t Cache::getBlockOffset(uint64_t address) const {
    return address % block_size;
}

bool Cache::access(uint64_t address, bool& hit) {
    current_time++;
    
    size_t set_index = getSetIndex(address);
    uint64_t tag = getTag(address);
    
    auto& set = sets_data[set_index];
    
    // --------- HIT CHECK ----------
    for (int way = 0; way < associativity; way++) {
        if (set[way].valid && set[way].tag == tag) {
            hit = true;
            hits++;
            updateAccess(set_index, way, true);
            return true;
        }
    }
    
    // --------- MISS ----------
    hit = false;
    misses++;
    
    // Find an invalid line first
    int victim_way = -1;
    for (int way = 0; way < associativity; way++) {
        if (!set[way].valid) {
            victim_way = way;
            break;
        }
    }
    
    // If all lines are valid, use replacement policy
    if (victim_way == -1) {
        if (policy == "LRU") {
            victim_way = findLRUVictim(static_cast<int>(set_index));  // Cast to int
        } else if (policy == "LFU") {
            victim_way = findLFUVictim(static_cast<int>(set_index));  // Cast to int
        } else if (policy == "FIFO") {
            victim_way = findFIFOVictim(static_cast<int>(set_index)); // Cast to int
        } else {
            // Default to LRU
            victim_way = findLRUVictim(static_cast<int>(set_index));  // Cast to int
        }
    }
    
    // Replace the victim line
    set[victim_way].valid = true;
    set[victim_way].tag = tag;
    set[victim_way].timestamp = current_time;
    
    updateAccess(static_cast<int>(set_index), victim_way, false);  // Cast to int
    
    return false;
}

int Cache::findLRUVictim(int set_index) {
    // The first element in the list is LRU
    int lru_way = lru_order[set_index].front();
    lru_order[set_index].pop_front();
    return lru_way;
}

int Cache::findLFUVictim(int set_index) {
    auto& set = sets_data[set_index];
    int victim_way = 0;
    int min_freq = set[0].freq;
    
    for (int way = 1; way < associativity; way++) {
        if (set[way].freq < min_freq) {
            min_freq = set[way].freq;
            victim_way = way;
        }
    }
    
    return victim_way;
}

int Cache::findFIFOVictim(int set_index) {
    int fifo_way = fifo_order[set_index].front();
    fifo_order[set_index].pop();
    return fifo_way;
}

void Cache::updateAccess(int set_index, int way, bool is_hit) {
    if (policy == "LRU") {
        // Move this way to the back (most recently used)
        auto& lru_list = lru_order[set_index];
        
        // Remove from current position
        for (auto it = lru_list.begin(); it != lru_list.end(); ++it) {
            if (*it == way) {
                lru_list.erase(it);
                break;
            }
        }
        
        // Add to back (MRU position)
        lru_list.push_back(way);
    } 
    else if (policy == "LFU" && is_hit) {
        sets_data[set_index][way].freq++;
    }
    else if (policy == "FIFO" && !is_hit) {
        // For FIFO, only update on miss (when line is added)
        fifo_order[set_index].push(way);
    }
}

void Cache::report() const {
    int total_accesses = hits + misses;
    double hit_rate = getHitRate();
    
    std::cout << "\n===== Cache Statistics =====\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";
    std::cout << "Total accesses: " << total_accesses << "\n";
    std::cout << "Hit rate: " << hit_rate << "%\n";
    std::cout << "Miss rate: " << (100.0 - hit_rate) << "%\n";
    std::cout << "Policy: " << policy << "\n";
    std::cout << "=============================\n";
}