#include "memory.hpp"
#include "allocator.hpp"
#include "cache.hpp"
#include "stats.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <random>

// Global counters for cache statistics
int L1_hits = 0, L1_misses = 0;
int L2_hits = 0, L2_misses = 0;

// Function to simulate cache access during memory operations
void simulateCacheAccess(
    std::vector<std::unique_ptr<Cache>>& caches,
    uint64_t address
) {
    bool hit;
    
    // Access L1 cache
    if (caches.size() >= 1 && caches[0]) {
        caches[0]->access(address, hit);
        if (hit) {
            L1_hits++;
            return; // L1 hit, no need to go to L2
        }
        L1_misses++;
    }
    
    // Access L2 cache (if exists)
    if (caches.size() >= 2 && caches[1]) {
        caches[1]->access(address, hit);
        if (hit) {
            L2_hits++;
            // Bring block into L1 (write allocation policy)
            bool dummy;
            caches[0]->access(address, dummy);
            return;
        }
        L2_misses++;
    }
    
    // If no cache or cache miss, simulate memory access
    // (Memory access latency could be simulated here)
}

// Generate a random address within memory bounds for simulation
uint64_t generateRandomAddress(size_t memory_size, size_t block_size) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, memory_size - block_size);
    
    // Align to block size
    uint64_t addr = dis(gen);
    return addr - (addr % block_size);
}

int main()
{
    std::unique_ptr<Memory> mem;
    std::unique_ptr<Allocator> alloc;
    std::vector<std::unique_ptr<Cache>> caches;
    int next_id = 1;
    
    // Reset global counters
    L1_hits = L1_misses = L2_hits = L2_misses = 0;

    std::cout << "Memory Management Simulator CLI" << std::endl;
    std::cout << "Type 'help' for commands or 'exit' to quit." << std::endl;

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit")
            break;
        else if (cmd == "help")
        {
            std::cout << "Commands:" << std::endl;
            std::cout << "  init memory <size>" << std::endl;
            std::cout << "  set allocator <first_fit|best_fit|worst_fit>" << std::endl;
            std::cout << "  malloc <size>" << std::endl;
            std::cout << "  free <id>" << std::endl;
            std::cout << "  dump memory" << std::endl;
            std::cout << "  stats" << std::endl;
            std::cout << "  init cache <level> <size> <block_size> <associativity> <policy>" << std::endl;
            std::cout << "  cache read <hex_address>    # Test cache read" << std::endl;
            std::cout << "  cache write <hex_address>   # Test cache write" << std::endl;
            std::cout << "  cache stats                 # Detailed cache stats" << std::endl;
            std::cout << "  cache test <num_accesses>   # Generate random cache accesses" << std::endl;
            std::cout << "  exit" << std::endl;
        }
        else if (cmd == "init" && iss >> cmd)
        {
            if (cmd == "memory")
            {
                size_t size;
                if (iss >> size)
                {
                    mem = std::make_unique<Memory>(size);
                    // Reset cache counters when memory is reinitialized
                    L1_hits = L1_misses = L2_hits = L2_misses = 0;
                    std::cout << "Memory initialized with size " << size << std::endl;
                }
                else
                    std::cout << "Error: Invalid memory size" << std::endl;
            }
            else if (cmd == "cache")
            {
                int level;
                size_t size, block_size;
                int associativity;
                std::string policy;
                if (iss >> level >> size >> block_size >> associativity >> policy)
                {
                    if (level >= 1 && level <= 3)
                    {
                        while (static_cast<int>(caches.size()) < level)
                            caches.emplace_back(nullptr);
                        caches[level - 1] = std::make_unique<Cache>(size, block_size, associativity, policy);
                        std::cout << "Cache L" << level << " initialized" << std::endl;
                        // Reset counters for this cache level
                        if (level == 1) {
                            L1_hits = L1_misses = 0;
                        } else if (level == 2) {
                            L2_hits = L2_misses = 0;
                        }
                    }
                    else
                        std::cout << "Error: Invalid cache level (1-3)" << std::endl;
                }
                else
                    std::cout << "Error: Invalid cache parameters" << std::endl;
            }
            else
                std::cout << "Error: Unknown init subcommand" << std::endl;
        }
        else if (cmd == "set" && iss >> cmd)
        {
            if (cmd == "allocator")
            {
                std::string type;
                if (iss >> type && mem)
                {
                    if (type == "first_fit")
                        alloc = std::make_unique<FirstFit>();
                    else if (type == "best_fit")
                        alloc = std::make_unique<BestFit>();
                    else if (type == "worst_fit")
                        alloc = std::make_unique<WorstFit>();
                    else if (type == "buddy")
                        alloc = std::make_unique<BuddyAllocator>(mem->getTotalSize());
                    else
                    {
                        std::cout << "Error: Unknown allocator type" << std::endl;
                        continue;
                    }
                    std::cout << "Allocator set to " << type << std::endl;
                }
                else
                    std::cout << "Error: Initialize memory first" << std::endl;
            }
            else
                std::cout << "Error: Unknown set subcommand" << std::endl;
        }
        else if (cmd == "malloc")
        {
            size_t size;
            if (iss >> size && mem && alloc)
            {
                // Simulate cache accesses during allocation
                if (caches.size() >= 1 && caches[0]) {
                    // Simulate reading free list metadata (simulated address)
                    simulateCacheAccess(caches, 0x1000);
                    
                    // Simulate writing block header (simulated address)
                    // We use a hash of the block ID as a simulated address
                    uint64_t header_addr = 0x2000 + (next_id * 64);
                    simulateCacheAccess(caches, header_addr);
                    
                    // Simulate zeroing out the allocated memory
                    for (size_t i = 0; i < size; i += caches[0]->getBlockSize()) {
                        uint64_t data_addr = 0x3000 + (next_id * 256) + i;
                        simulateCacheAccess(caches, data_addr);
                    }
                }
                
                Block *block = alloc->allocate(*mem, size, next_id);
                if (block)
                {
                    std::cout << "Allocated block id=" << next_id 
                              << " at address=0x" << std::hex 
                              << reinterpret_cast<uintptr_t>(block) << std::dec << std::endl;
                    ++next_id;
                }
                else
                    std::cout << "Allocation failed" << std::endl;
            }
            else
                std::cout << "Error: Initialize memory and allocator first" << std::endl;
        }
        else if (cmd == "free")
        {
            int id;
            if (iss >> id && mem)
            {
                // Simulate cache accesses during deallocation
                if (caches.size() >= 1 && caches[0]) {
                    // Simulate reading block header
                    uint64_t header_addr = 0x2000 + (id * 64);
                    simulateCacheAccess(caches, header_addr);
                    
                    // Simulate writing free list update
                    simulateCacheAccess(caches, 0x1000);
                }
                
                mem->deallocate(id);
                std::cout << "Block " << id << " freed and merged" << std::endl;
            }
            else
                std::cout << "Error: Initialize memory first" << std::endl;
        }
        else if (cmd == "dump" && iss >> cmd)
        {
            if (cmd == "memory" && mem)
                mem->dump();
            else
                std::cout << "Error: Initialize memory first" << std::endl;
        }
        else if (cmd == "stats")
        {
            if (mem)
                Stats::report(*mem);
            else
                std::cout << "Error: Initialize memory first" << std::endl;
            
            // Show cache statistics
            std::cout << "\n===== Cache Statistics =====\n";
            if (caches.size() >= 1 && caches[0]) {
                std::cout << "L1 Cache Hits: " << L1_hits << std::endl;
                std::cout << "L1 Cache Misses: " << L1_misses << std::endl;
                int L1_total = L1_hits + L1_misses;
                double L1_hit_rate = (L1_total > 0) ? (double)L1_hits / L1_total * 100.0 : 0.0;
                std::cout << "L1 Hit Rate: " << L1_hit_rate << "%" << std::endl;
            }
            if (caches.size() >= 2 && caches[1]) {
                std::cout << "L2 Cache Hits: " << L2_hits << std::endl;
                std::cout << "L2 Cache Misses: " << L2_misses << std::endl;
                int L2_total = L2_hits + L2_misses;
                double L2_hit_rate = (L2_total > 0) ? (double)L2_hits / L2_total * 100.0 : 0.0;
                std::cout << "L2 Hit Rate: " << L2_hit_rate << "%" << std::endl;
            }
            std::cout << "==============================\n";
        }
        else if (cmd == "cache" && iss >> cmd)
        {
            if (cmd == "read") {
                uint64_t address;
                if (iss >> std::hex >> address >> std::dec && !caches.empty() && caches[0]) {
                    simulateCacheAccess(caches, address);
                    std::cout << "Cache read at 0x" << std::hex << address << std::dec << std::endl;
                } else {
                    std::cout << "Error: Initialize cache first" << std::endl;
                }
            }
            else if (cmd == "write") {
                uint64_t address;
                if (iss >> std::hex >> address >> std::dec && !caches.empty() && caches[0]) {
                    simulateCacheAccess(caches, address);
                    std::cout << "Cache write at 0x" << std::hex << address << std::dec << std::endl;
                } else {
                    std::cout << "Error: Initialize cache first" << std::endl;
                }
            }
            else if (cmd == "stats") {
                if (!caches.empty() && caches[0]) {
                    caches[0]->report();
                    if (caches.size() >= 2 && caches[1]) {
                        caches[1]->report();
                    }
                } else {
                    std::cout << "Error: Initialize cache first" << std::endl;
                }
            }
            else if (cmd == "test") {
                int num_accesses;
                if (iss >> num_accesses && mem && !caches.empty() && caches[0]) {
                    std::cout << "Generating " << num_accesses << " random cache accesses..." << std::endl;
                    for (int i = 0; i < num_accesses; i++) {
                        uint64_t addr = generateRandomAddress(mem->getTotalSize(), caches[0]->getBlockSize());
                        simulateCacheAccess(caches, addr);
                    }
                    std::cout << "Cache test completed." << std::endl;
                } else {
                    std::cout << "Error: Initialize memory and cache first" << std::endl;
                }
            }
            else {
                std::cout << "Error: Unknown cache command" << std::endl;
            }
        }
        else
            std::cout << "Error: Unknown command" << std::endl;
    };

    std::cout << "Exiting simulator." << std::endl;
    return 0;
}