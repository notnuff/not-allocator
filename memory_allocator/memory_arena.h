#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

// size of arena measured in pages
#define DEFAULT_ARENA_SIZE 15

// default size of one page - 4kb
#define DEFAULT_PAGE_SIZE 4096


// so, the main idea - this struct will be saved in the header of some memory chunk,
// and that`s how memory arena will be defined
struct memory_arena_t {
    size_t size;
    struct memory_arena* next;
} typedef memory_arena_t;

constexpr auto MEMORY_ARENA_HEADER_SIZE = sizeof(memory_arena_t);

#endif //MEMORY_ARENA_H
