#ifndef KERNEL_ALLOCATOR_H
#define KERNEL_ALLOCATOR_H

#include <cstddef>

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

#endif //KERNEL_ALLOCATOR_H
