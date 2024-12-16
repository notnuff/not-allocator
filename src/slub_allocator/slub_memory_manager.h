#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "memory_cache.h"


class SlubMemoryManager {
public:

  // num_of_small - number of small objects caches,
  // with size step of one alignment (16 bytes)

  // num_per_group - number of elements in one group,
  // stepping in the group = previous group stepping * 2

  // num_of_exponential_groups - number of groups with increasing stepping

  // given num_of_small = 8, num_per_group = 4, num_of_exponential_groups = 3,
  // we have next caches:
  // 16, 32, 48, 64, 80, 96, 112, 128, - first 8 caches with stepping of 16 bytes
  // 160, 192, 224, 256, - first group with stepping of 32 bytes
  // 320, 384, 448, 512, - second group with stepping of 64 bytes
  // 640, 768, 896, 1024 - third group with stepping of 128 bytes

  SlubMemoryManager(size_t num_of_small, size_t num_per_group, size_t num_of_exponential_groups);

  void* Allocate(size_t size);
  void* Reallocate(void* ptr, size_t size);
  void Free(void* ptr);

protected:
  MemorySlab* GetSlabOfPtr(void* ptr);
  MemoryCache* CalculateCacheForSize(size_t size);

  MemoryCache* caches_;
  size_t num_of_caches_;
};


#endif //MEMORY_MANAGER_H
