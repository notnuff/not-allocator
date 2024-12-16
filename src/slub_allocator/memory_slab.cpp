#include "memory_slab.h"

#include <general_memory_abstractions/memory_chunk.h>
#include <memory_allocators_apis/mem_allocator.h>
#include <slub_allocator/memory_cache.h>
#include <common/list.h>

#include <algorithm>
#include <cstdlib>
#include <new>
#include <stdexcept>


MemorySlab::MemorySlab(size_t obj_full_size) {
  auto calculated_slab_size = DEFAULT_PAGE_SIZE;
  AppendExtend(calculated_slab_size);
  PopulateExtend(obj_full_size);

  next_linked_slab_ = nullptr;
}

void MemorySlab::AppendExtend(size_t extend_size) {
  // by default, memory chunk is aligned for ANY data type,
  // as begin of page is always dividable by size of any primitive data type
  extend_begin_ = MemoryChunk::AppendNewMemoryChunk(extend_size);
  extend_size_ = extend_size;
}

void MemorySlab::PopulateExtend(size_t step_size) {
  free_objects_list_entry_ = new (extend_begin_) ListNodeT();

  auto min_obj_size = std::max(step_size, sizeof(ListNode<void>));
  if ( extend_size_ % min_obj_size != 0)
    throw std::runtime_error("Slab size must be a multiple of min_obj_size");

  auto* current_node = free_objects_list_entry_;

  for (size_t i = 0; i < extend_size_ / min_obj_size; ++i) {
    void* next_obj_ptr = (char*) current_node + min_obj_size;
    current_node->next_node_ = new (next_obj_ptr) ListNodeT();
    current_node = current_node->next_node_;
  }
}

MemorySlab* MemorySlab::CreateSlab(size_t obj_full_size, MemoryCache* cache) {
  auto* mem = mem_alloc(sizeof(MemorySlab));
  auto* slab = new (mem) MemorySlab(obj_full_size);
  slab->MemCache(cache);

  return slab;
}

void* MemorySlab::GetFreeObject() {
  auto* current_node = free_objects_list_entry_;
  if (!current_node) return nullptr;

  free_objects_list_entry_ = current_node->next_node_;
  return current_node;
}

void MemorySlab::ReturnObject(void* ptr) {
  if (ptr == nullptr) throw std::runtime_error("returned null pointer");
  if (!IsPtrBelongsToSlab(ptr)) throw std::runtime_error("pointer not belong to slab");

  auto* current_node = free_objects_list_entry_;
  auto* new_entry = new (ptr) ListNodeT();
  new_entry->next_node_ = current_node;

  free_objects_list_entry_ = new_entry;
}

bool MemorySlab::IsPtrBelongsToSlab(void* ptr) {
  auto* extend_begin = (char*)extend_begin_;
  auto* extend_end = extend_begin + extend_size_;
  auto* ptr_comparable = (char*)ptr;

  return ptr_comparable >= extend_begin && ptr_comparable <= extend_end;
}

MemorySlab* MemorySlab::NextSlab() { return next_linked_slab_; }

void MemorySlab::NextSlab(MemorySlab* ptr) { next_linked_slab_ = ptr; }

MemoryCache* MemorySlab::MemCache() { return cache_; }
void MemorySlab::MemCache(MemoryCache* ptr) { cache_ = ptr; }
