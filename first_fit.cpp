#include "allocator.hpp"

Block* FirstFit::allocate(Memory& mem, size_t size, int id) {
    return mem.allocate(size, id);
}