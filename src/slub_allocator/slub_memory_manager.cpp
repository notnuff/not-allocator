#include "slub_memory_manager.h"

#include <cstddef>
#include <cstring>
#include <list>
#include <new>

#include "common/custom_math.h"
#include "general_memory_abstractions/memory_chunk.h"
#include "memory_allocators_apis/mem_allocator.h"
#include "cache_for_caches.h"


MemoryCache* SlubMemoryManager::AllocateCache(const MemoryCacheInitParams& init_params) {
  auto* mem = getCacheForCaches().AllocateObject();
  return new (mem) MemoryCache(init_params) ;
}

SlubMemoryManager::SlubMemoryManager(size_t num_of_small, size_t num_per_group,
                                     size_t num_of_exponential_groups) {

  /* here is little issue with dynamic approach for creating memory caches:
   * at init, we have only cache_for_caches,
   * in which we will hold all structures of caches
   * SO, we basically can't create any continuous chunk of memory,
   * where we will hold array with caches, so we basically can't create an array
   * for that purposes, BUTT
   * what if we create a cache that can hold such array?
  */


  num_of_caches_ = num_of_small + num_of_exponential_groups * num_per_group;
  cache_for_array_ = AllocateCache({
    sizeof(MemoryCache) * num_of_caches_,
    alignof(MemoryCache),
    false});

  allocated_caches_ = (MemoryCache*) cache_for_array_->AllocateObject();

  auto current_group_step = alignof(max_align_t);

  for (size_t i = 0; i < num_of_small; i++) {
    auto cache_size = current_group_step * (i + 1);

    // toask: that are basically identical things, aren't they?
    // auto* cache = new(&allocated_caches_[i]) MemoryCache({cache_size});
    allocated_caches_[i] = MemoryCache({cache_size});
  }

  for (size_t i = 0; i < num_of_exponential_groups; i++) {
    auto current_group_index = num_of_small + i;
    current_group_step *= 2;
    for (size_t j = 0; j < num_per_group; j++) {
      auto offset_in_current_group = j * current_group_step;
      auto current_cache_size =
          current_group_index * current_group_step + offset_in_current_group;
      allocated_caches_[current_group_index + j] = MemoryCache({current_cache_size});
    }
  }
}

void* SlubMemoryManager::Allocate(size_t size) {
  auto* cache = CalculateCacheForSize(size);

  if (!cache) {
    // cant find appropriate cache - so object is too big,
    // so allocate it from general allocator
    return nullptr;
  }

  return cache->AllocateObject();
}

MemorySlab* SlubMemoryManager::GetSlabOfPtr(void* ptr) {
  for (size_t i = 0; i < num_of_caches_; i++) {
    auto* slab = allocated_caches_[i].SlabOfMemory(ptr);
    if (slab) return slab;
  }
  return nullptr;
}

void* SlubMemoryManager::Reallocate(void* ptr, size_t size) {
  auto* slab = GetSlabOfPtr(ptr);
  if (!slab) return mem_realloc(ptr, size);

  auto* new_ptr = Allocate(size);
  memcpy( new_ptr, ptr, slab->MemCache()->MaxObjSize() );

  Free(ptr);

  return new_ptr;
}

void SlubMemoryManager::Free(void* ptr) {
  for (size_t i = 0; i < num_of_caches_; i++) {
    if (allocated_caches_[i].TryReturnObject(ptr) ) return;
  }

  mem_free(ptr);
}

MemoryCache* SlubMemoryManager::CalculateCacheForSize(size_t size) {
  constexpr auto align = sizeof(max_align_t);
  size = round_up(size, align);

  // well, I really would like to calculate it, like really very much,
  // but no, I don't care, I'll just iterate over all caches because I'm too lazy

  for (size_t i = 0; i < num_of_caches_; i++) {
    if (allocated_caches_[i].MaxObjSize() >= size) return &allocated_caches_[i];
  }

  return nullptr;
}

