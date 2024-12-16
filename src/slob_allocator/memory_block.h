#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

#include <cstddef>

enum class BLOCK_STATUS {
    FREE = 0,
    OCCUPIED = 1,
};

class MemoryBlock {
public:
    static MemoryBlock* ConvertToMemoryBlock(void* start, size_t size);

    size_t Size();
    void Size(size_t size);

    MemoryBlock* Next();
    void Next(MemoryBlock* next);

    MemoryBlock* Prev();
    void Prev(MemoryBlock* prev);

    BLOCK_STATUS Tag();
    void Tag(BLOCK_STATUS block_status);


    void* UserSpace();
    size_t UserSpaceSize();

    static size_t HeaderSize();

protected:
    size_t size_ = 0;
    BLOCK_STATUS tag = BLOCK_STATUS::FREE;

    MemoryBlock* prev_block_begin = nullptr;
    MemoryBlock* next_block_begin = nullptr;
};


#endif //MEMORY_BLOCK_H
