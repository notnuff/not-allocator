#ifndef MEMORY_SLAB_H
#define MEMORY_SLAB_H

#include "memory_constants.h"

class MemorySlab {
    public:

};

class MemoryCache {
public:
    MemorySlab* slabs_full;
    MemorySlab* slabs_partial;
    MemorySlab* slabs_free;

    size_t object_size;

};

#endif //MEMORY_SLAB_H
