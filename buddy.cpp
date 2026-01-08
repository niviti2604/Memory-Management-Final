#include "allocator.hpp"
#include <cmath>

BuddyAllocator::BuddyAllocator(size_t mem_size) : mem_size(mem_size) {
    size_t max_order = std::log2(mem_size);
    free_lists.resize(max_order + 1);
    free_lists[max_order].push_back(new Block{mem_size,0, true, -1, nullptr});
}

size_t BuddyAllocator::get_power_of_two(size_t size) {
    size_t p = 1;
    while (p < size) p <<= 1;
    return p;
}

Block* BuddyAllocator::allocate(Memory& mem, size_t size, int id) {
    size_t alloc_size = get_power_of_two(size);
    int order = std::log2(alloc_size);
    for (size_t i = order; i <= free_lists.size(); ++i) {
        if (!free_lists[i].empty()) {
            Block* block = free_lists[i].front();
            free_lists[i].pop_front();
            while (i > (size_t)order) {
                size_t half = block->size / 2;
                Block* buddy = new Block{half, 0, true, -1, nullptr};
                free_lists[i-1].push_back(buddy);
                block->size = half;
                --i;
            }
            block->free = false;
            block->id = id;
            return block;
        }
    }
    return nullptr;
}

void BuddyAllocator::deallocate(Memory& mem, int id) {
}