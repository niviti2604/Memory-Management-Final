#include "allocator.hpp"

Block* BestFit::allocate(Memory& mem, size_t size, int id) {
    return mem.allocateWithSelect(size, id, [](Block* a, Block* b) {
        return (a->size < b->size) ? a : b;
    });
}