#include "memory_slab.h"

#include <common/list.h>
#include <general_memory_abstractions/memory_chunk.h>
#include <memory_allocators_apis/mem_allocator.h>
#include <slub_allocator/memory_cache.h>

#include <algorithm>
#include <cstdlib>
#include <new>
#include <stdexcept>

#include "cache_for_caches.h"
#include "common/custom_math.h"

MemorySlab::MemorySlab(size_t obj_full_size) {
  auto calculated_slab_size = CalculateExtendSize(obj_full_size);
  AppendExtend(calculated_slab_size);
  PopulateExtend(obj_full_size);

  next_linked_slab_ = nullptr;
}

size_t MemorySlab::CalculateExtendSize(size_t obj_size){
  return round_up(obj_size, DEFAULT_PAGE_SIZE);
}

MemorySlab* MemorySlab::CreateSlabInlined(size_t obj_full_size) {
  constexpr auto slab_aligned_size =
    sizeof(MemorySlab) + alignof(MemorySlab);

  auto extend_required_space =
    CalculateExtendSize( obj_full_size + slab_aligned_size);

  auto* extend_header_begin = MemoryChunk::AppendNewMemoryChunk(extend_required_space);
  auto* extend_slab_begin = (char*) extend_header_begin + slab_aligned_size;
  auto extend_size = extend_required_space - slab_aligned_size;

  auto* slab = (MemorySlab*) extend_header_begin;
  slab->extend_begin_ = extend_slab_begin;
  slab->extend_size_ = extend_size;

  slab->PopulateExtend(obj_full_size);
  return slab;
}


MemorySlab* MemorySlab::CreateSlabExternal(size_t obj_full_size,
                                           void* mem_for_slab) {
  return new (mem_for_slab) MemorySlab(obj_full_size);
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
  // if ( extend_size_ % min_obj_size != 0)
  //   throw std::runtime_error("Slab size must be a multiple of min_obj_size");

  if ( extend_size_ < min_obj_size )
    throw std::runtime_error("Slab size must be bigger than min_obj_size");

  auto* current_node = free_objects_list_entry_;

  for (size_t i = 0; i < extend_size_ / min_obj_size; ++i) {
    void* next_obj_ptr = (char*) current_node + min_obj_size;
    current_node->next_node_ = new (next_obj_ptr) ListNodeT();
    current_node = current_node->next_node_;
  }
}

MemorySlab* MemorySlab::CreateSlab(const MemorySlabInitParams& init_params) {
  MemorySlab* slab = nullptr;

  if (init_params.inline_header) {
    slab = CreateSlabInlined(init_params.object_size);
  } else {
    auto* slab_mem = getCacheForSlabHeaders().AllocateObject();
    slab = CreateSlabExternal(init_params.object_size, slab_mem);
  }

  if (!slab) throw std::bad_alloc();

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
