#include "memory_arena.h"


MemoryArena* MemoryArena::ConvertToMemoryArena(void *start, const size_t size) {
    auto* arena = (MemoryArena*) start;
    arena->size_ = size;
    arena->next_ = NULL;

    return arena;
}

size_t MemoryArena::Size() {
    return size_;
}

void MemoryArena::Size(const size_t size) {
    size_ = size;
}

MemoryArena* MemoryArena::Next() {
    return next_;
}

void MemoryArena::Next(MemoryArena *next) {
    next_ = next;
}

void* MemoryArena::UserSpace() {
    return (void*) ((char*) this + HeaderSize());
}

size_t MemoryArena::UserSpaceSize() {
    return size_ - HeaderSize();
}

size_t MemoryArena::HeaderSize() {
    return sizeof(MemoryArena);
}
