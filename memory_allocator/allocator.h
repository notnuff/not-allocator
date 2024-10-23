#ifndef LAB1_MEMORY_ALLOCATOR_ALLOCATOR_H_
#define LAB1_MEMORY_ALLOCATOR_ALLOCATOR_H_

#include <cstddef>

void *mem_alloc(size_t size);
void mem_free(void* ptr);
void *mem_realloc(void* ptr, size_t new_size);

#endif //LAB1_MEMORY_ALLOCATOR_ALLOCATOR_H_
