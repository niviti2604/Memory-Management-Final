#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include "memory.hpp" 
#include <functional>

class Allocator {
public:
    virtual Block* allocate(Memory& mem, size_t size, int id) = 0;
    virtual ~Allocator() {}
};

class FirstFit : public Allocator {
public:
    Block* allocate(Memory& mem, size_t size, int id) override;
};

class BestFit : public Allocator {
public:
    Block* allocate(Memory& mem, size_t size, int id) override;
};

class WorstFit : public Allocator {
public:
    Block* allocate(Memory& mem, size_t size, int id) override;
};

class BuddyAllocator : public Allocator {
public:
    BuddyAllocator(size_t mem_size);
    Block* allocate(Memory& mem, size_t size, int id) override;
    void deallocate(Memory& mem, int id);
private:
    size_t mem_size;
    std::vector<std::list<Block*>> free_lists;
    size_t get_power_of_two(size_t size);
};

#endif