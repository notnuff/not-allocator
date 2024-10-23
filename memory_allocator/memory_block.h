#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H


enum class BLOCK_STATUS {
    FREE = 0,
    OCCUPIED = 1,
};

struct memory_block_t {
    BLOCK_STATUS tag;
    size_t size;

    memory_block_t* prev_block_begin;
    memory_block_t* next_block_begin;
} typedef memory_block_t;

constexpr auto MEMORY_BLOCK_HEADER_SIZE = sizeof(memory_block_t);

#endif //MEMORY_BLOCK_H
