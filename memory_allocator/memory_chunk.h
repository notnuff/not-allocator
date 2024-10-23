#ifndef LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_
#define LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_


// it is highly recommended to set size with (numOfDesiredPages * DEFAULT_PAGE_SIZE)
void* get_new_memory_chunk(size_t bytes);
void remove_arena(void* ptr);

#endif //LAB1_MEMORY_ALLOCATOR_MEMORY_CHUNK_H_
