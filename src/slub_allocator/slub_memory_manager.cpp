#include "slub_memory_manager.h"

#include <cstddef>
#include <cstring>
#include <list>
#include <new>

#include "common/custom_math.h"
#include "general_memory_abstractions/memory_chunk.h"
#include "memory_allocators_apis/mem_allocator.h"

static MemoryCache cache_for_caches( sizeof(MemoryCache) );


SlubMemoryManager::SlubMemoryManager(size_t num_of_small, size_t num_per_group,
                                     size_t num_of_exponential_groups) {
  auto num_of_caches = num_of_small + num_of_exponential_groups * num_per_group;

  auto size_of_aligned_cache = round_up(sizeof(MemoryCache), alignof(MemoryCache));
  auto* mem = (MemoryCache*) mem_alloc(num_of_caches * size_of_aligned_cache);

  caches_ = mem;
  num_of_caches_ = num_of_caches;

  auto current_group_step = alignof(max_align_t);

  for (size_t i = 0; i < num_of_small; i++) {
    caches_[i] = MemoryCache(current_group_step * (i + 1));
  }

  for (size_t i = 0; i < num_of_exponential_groups; i++) {
    auto current_group_index = num_of_small + i;
    current_group_step *= 2;
    for (size_t j = 0; j < num_per_group; j++) {
      auto offset_in_current_group = j * current_group_step;
      auto current_cache_size =
          current_group_index * current_group_step + offset_in_current_group;
      caches_[current_group_index + j] = MemoryCache(current_cache_size);
    }
  }
}

void* SlubMemoryManager::Allocate(size_t size) {
  auto* cache = CalculateCacheForSize(size);

  if (!cache) {
    // cant find appropriate cache - so object is too big,
    // so allocate it from general allocator
    return mem_alloc(size);
  }

  return cache->AllocateObject();
}

MemorySlab* SlubMemoryManager::GetSlabOfPtr(void* ptr) {
  for (size_t i = 0; i < num_of_caches_; i++) {
    auto* slab = caches_[i].SlabOfMemory(ptr);
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
    if (caches_[i].TryReturnObject(ptr) ) return;
  }

  mem_free(ptr);
}

MemoryCache* SlubMemoryManager::CalculateCacheForSize(size_t size) {
  constexpr auto align = sizeof(max_align_t);
  size = round_up(size, align);

  // well, I really would like to calculate it, like really very much,
  // but no, I don't care, i'll just iterate over all caches because i'm too lazy

  for (size_t i = 0; i < num_of_caches_; i++) {
    if (caches_[i].MaxObjSize() >= size) return &caches_[i];
  }

  return nullptr;
}

