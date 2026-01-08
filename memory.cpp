#include "memory.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

Memory::Memory(size_t size)
    : total_size(size), data(new uint8_t[size])
{
    head = new Block{size, 0, true, -1, nullptr};
}

Memory::~Memory()
{
    delete[] data;
    while (head)
    {
        Block *tmp = head;
        head = head->next;
        delete tmp;
    }
}

Block *Memory::allocate(size_t size, int id)
{
    const size_t MIN_SPLIT_THRESHOLD = 32;
    alloc_requests++;

    Block *curr = head;
    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            if (curr->size >= size + MIN_SPLIT_THRESHOLD)
            {
                Block *new_block = new Block{
                    curr->size - size,
                    0,
                    true,
                    -1,
                    curr->next};
                curr->next = new_block;
                curr->size = size;
            }

            curr->free = false;
            curr->id = id;
            curr->requested_size = size;
            alloc_success++;

            return curr;
        }
        curr = curr->next;
    }
    alloc_failure++;
    return nullptr;
}

Block *Memory::allocateWithSelect(size_t size,int id,std::function<Block *(Block *, Block *)> select)
{
    alloc_requests++;

    Block *block = findFreeBlock(size, select);
    if (!block)
    {
        alloc_failure++;
        return nullptr;
    }

    if (block->size > size)
    {
        Block *new_block = new Block{ block->size - size,0,true,-1,block->next};
        block->next = new_block;
        block->size = size;
    }

    block->free = false;
    block->id = id;
    block->requested_size = size;

    alloc_success++;

    return block;
}

Block *Memory::findFreeBlock(size_t size,std::function<Block *(Block *, Block *)> select)
{

    Block *candidate = nullptr;
    Block *curr = head;

    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            if (!candidate)
                candidate = curr;
            else
                candidate = select(curr, candidate);
        }
        curr = curr->next;
    }
    return candidate;
}

void Memory::deallocate(int id)
{
    Block *curr = head;
    while (curr)
    {
        if (!curr->free && curr->id == id)
        {
            curr->free = true;
            curr->id = -1;
            curr->requested_size = 0;
            coalesce();
            return;
        }
        curr = curr->next;
    }
}

void Memory::coalesce()
{
    Block *curr = head;
    while (curr && curr->next)
    {
        if (curr->free && curr->next->free)
        {
            curr->size += curr->next->size;
            Block *tmp = curr->next;
            curr->next = tmp->next;
            delete tmp;
        }
        else
        {
            curr = curr->next;
        }
    }
}

void Memory::dump() const
{
    Block *curr = head;
    size_t addr = 0;
    std::cout << std::hex;

    while (curr)
    {
        std::cout << "[0x" << std::hex << addr
                  << " - 0x" << (addr + curr->size - 1) << "] ";

        if (curr->free)
            std::cout << "FREE";
        else
            std::cout << "USED (id=" << curr->id
                      << ", req=" << curr->requested_size << ")";

        std::cout << std::dec << "\n";

        addr += curr->size;
        curr = curr->next;
    }
    std::cout << std::dec;
}

size_t Memory::getUsedSize() const
{
    size_t used = 0;
    Block *curr = head;
    while (curr)
    {
        if (!curr->free)
            used += curr->size;
        curr = curr->next;
    }
    return used;
}

double Memory::getExternalFragmentation() const
{
    size_t free_total = 0, largest_free = 0;
    Block *curr = head;

    while (curr)
    {
        if (curr->free)
        {
            free_total += curr->size;
            largest_free = std::max(largest_free, curr->size);
        }
        curr = curr->next;
    }

    return free_total == 0 ? 0.0 : 100.0 * (1.0 - (double)largest_free / free_total);
}

double Memory::getInternalFragmentation() const
{
    size_t internal = 0;
    Block *curr = head;

    while (curr)
    {
        if (!curr->free)
            internal += (curr->size - curr->requested_size);
        curr = curr->next;
    }
    return internal;
}

void Memory::stats() const {
    size_t used = getUsedSize();

    cout << "Total memory: " << total_size << "\n";
    cout << "Used memory: " << used << "\n";

    cout << "Internal fragmentation: "
         << getInternalFragmentation() << "%\n";

    cout << "External fragmentation: "
         << getExternalFragmentation() << "%\n";

    // ===== Feature 4 additions =====
    double success_rate =
        (alloc_requests == 0) ? 0 :
        (alloc_success * 100.0) / alloc_requests;

    double failure_rate =
        (alloc_requests == 0) ? 0 :
        (alloc_failure * 100.0) / alloc_requests;

    double utilization =
        (used * 100.0) / total_size;

    cout << "Allocation success rate: "
         << success_rate << "%\n";

    cout << "Allocation failure rate: "
         << failure_rate << "%\n";

    cout << "Memory utilization: "
         << utilization << "%\n";
}
