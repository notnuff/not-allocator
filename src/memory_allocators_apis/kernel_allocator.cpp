#include "kernel_allocator.h"

#include "slub_allocator/slub_memory_manager.h"


# define NUM_OF_SMALL 8
# define NUM_PER_GROUP 4
# define NUM_OF_EXPONENTIAL_GROUPS 7

static SlubMemoryManager slub_memory_manager(
  NUM_OF_SMALL,
  NUM_PER_GROUP,
  NUM_OF_EXPONENTIAL_GROUPS
  );

void* kmalloc(size_t size) {
  return slub_memory_manager.Allocate(size);
}

void* krealloc(void* ptr, size_t size) {
  return slub_memory_manager.Reallocate(ptr, size);
}

void kfree(void* ptr) {
  return slub_memory_manager.Free(ptr);

}
