#ifndef LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_
#define LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_

// it is highly recommended to set size with (numOfDesiredPages * DEFAULT_PAGE_SIZE)

// default size of one page - 4kb
#define DEFAULT_PAGE_SIZE 4096

class MemoryChunk {
public:
    MemoryChunk()=delete;
    static void* AppendNewMemoryChunk(size_t bytes);
    static void RemoveMemoryChunk(void* chunk_ptr, size_t length);
};

#endif //LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_
