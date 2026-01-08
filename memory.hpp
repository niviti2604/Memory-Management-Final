#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <vector>
#include <cstdint>
#include <list>
#include <functional>

struct Block {
    size_t size;
    size_t requested_size;
    bool free;
    int id;
    Block* next;
};

class Memory {
public:
    Memory(size_t size);
    ~Memory();

    Block* allocate(size_t size, int id);
    Block* allocateWithSelect(size_t size, int id,
        std::function<Block*(Block*, Block*)> select);

    void deallocate(int id);
    void dump() const;
    void stats() const;

    size_t getTotalSize() const { return total_size; }
    size_t getUsedSize() const;
    double getExternalFragmentation() const;
    double getInternalFragmentation() const;

    const Block* getHead() const { return head; }

private:
    size_t total_size;
    uint8_t* data;
    Block* head;

    size_t alloc_requests = 0;
    size_t alloc_success  = 0;
    size_t alloc_failure  = 0;

    Block* findFreeBlock(size_t size, std::function<Block*(Block*, Block*)> select);
    void coalesce();
};

#endif
