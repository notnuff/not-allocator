#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H

#include <slub_allocator/memory_slab.h>

class MemoryCache {
public:
  static MemoryCache* CreateCache(size_t size, size_t alignment = 0);

  void* AllocateObject();
  bool TryReturnObject(void* object_ptr);

  size_t MaxObjSize() const;
  MemorySlab* SlabOfMemory(void* ptr);

public:
  explicit MemoryCache(size_t size, size_t alignment = 0);

protected:
  MemorySlab* CreateNewSlab();
  void SwapCurrentSlabToFull();
  void SwapPartialSlabToCurrent();
  void SwapSlabToPartial(MemorySlab* slab);

protected:
  size_t object_size_;

protected:
  MemorySlab* current_slab_;

protected:

  // do I really need this? probably not
  class MemoryCacheNode {
  public:
    MemorySlab* slab_list_partial_;
    MemorySlab* slab_list_full_;
  };

  MemoryCacheNode* cache_node_;
};



#endif //MEMORY_CACHE_H
