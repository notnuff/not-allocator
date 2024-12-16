#ifndef MEMORY_SLAB_H
#define MEMORY_SLAB_H

#include <common/list.h>

class MemoryCache;
class MemorySlab {
public:
  // obj_full_size should be ALIGNED already
  // minimal obj_full_size
  static MemorySlab* CreateSlab(size_t obj_full_size, MemoryCache* cache = nullptr);

  void* GetFreeObject();
  void ReturnObject(void* ptr);

  bool IsPtrBelongsToSlab(void* ptr);

public:
  MemorySlab* NextSlab();
  void NextSlab(MemorySlab* ptr);

  MemoryCache* MemCache();
  void MemCache(MemoryCache* ptr);

protected:
  explicit MemorySlab(size_t obj_full_size);

  void AppendExtend(size_t extend_size);
  void PopulateExtend(size_t step_size);

  void* extend_begin_;
  size_t extend_size_;

  ListNodeT* free_objects_list_entry_;

  MemorySlab* next_linked_slab_;

  MemoryCache* cache_;
};

#endif //MEMORY_SLAB_H
