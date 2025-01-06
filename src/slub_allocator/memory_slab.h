#ifndef MEMORY_SLAB_H
#define MEMORY_SLAB_H

#include <common/list.h>
#include <cstddef>

class MemoryCache;

struct MemorySlabInitParams {
  size_t object_size = 0;
  bool inline_header = true;
  MemoryCache* cache = nullptr;
};

class MemorySlab {
 public:
  // obj_full_size should be ALIGNED already
  // minimal obj_full_size
  static MemorySlab* CreateSlab(const MemorySlabInitParams& init_params);

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

  static MemorySlab* CreateSlabInlined(size_t obj_full_size);
  static MemorySlab* CreateSlabExternal(size_t obj_full_size,
                                        void* mem_for_slab);

  void AppendExtend(size_t extend_size);
  void PopulateExtend(size_t step_size);

  static size_t CalculateExtendSize(size_t obj_size);

 protected:
  void* extend_begin_;
  size_t extend_size_;

  ListNodeT* free_objects_list_entry_;

  MemorySlab* next_linked_slab_;

  MemoryCache* cache_;

  DoubleLinkedListNode<MemorySlab> list_head_;
};

#endif //MEMORY_SLAB_H
