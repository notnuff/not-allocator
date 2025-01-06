#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H

#include <slub_allocator/memory_slab.h>

struct MemoryCacheInitParams {
  size_t object_size = 0;
  size_t alignment = 0;
  bool force_inline_header = true;
};

class MemoryCache {
public:
  explicit MemoryCache(const MemoryCacheInitParams& init_params);

  void* AllocateObject();
  bool TryReturnObject(void* object_ptr);

  size_t MaxObjSize() const;
  MemorySlab* SlabOfMemory(void* ptr);


protected:
  MemorySlab* CreateNewSlab();
  void SwapCurrentSlabToFull();
  void SwapPartialSlabToCurrent();
  void SwapSlabToPartial(MemorySlab* slab);

protected:
  size_t object_size_ = 0;

protected:
  MemorySlab* current_slab_ = nullptr;
  MemorySlab* slab_list_partial_ = nullptr;
  MemorySlab* slab_list_full_ = nullptr;

protected:
  // the flag which defines if the header of slab should
  // be located directly on beginning of slab
  bool force_inline_header_ = false;
};



#endif //MEMORY_CACHE_H
