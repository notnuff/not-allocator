#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include <cstddef>

// size of arena measured in pages
#define DEFAULT_ARENA_SIZE 15


// so, the main idea - this struct will be saved in the header of some memory chunk,
// and that`s how memory arena will be defined

class MemoryArena {
public:
    // we shouldn`t be able to simply create memory arenas
    MemoryArena()=delete;

    // instead, we need to create arenas from raw pointers and size
    static MemoryArena* ConvertToMemoryArena(void* start, size_t size);

    size_t Size();
    void Size(size_t size);

    MemoryArena* Next();
    void Next(MemoryArena* next);

    void* UserSpace();
    size_t UserSpaceSize();

    static size_t HeaderSize();

protected:
    size_t size_ = 0;
    MemoryArena* next_ = nullptr;
};

#endif //MEMORY_ARENA_H
