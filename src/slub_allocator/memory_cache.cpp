#include "memory_cache.h"

#include <memory_allocators_apis/mem_allocator.h>

#include <new>
#include <stdexcept>

#include "common/custom_math.h"

MemoryCache::MemoryCache(const MemoryCacheInitParams& init_params) {
  object_size_ = round_up(init_params.object_size, init_params.alignment);
  force_inline_header_ = init_params.force_inline_header;
}

MemorySlab* MemoryCache::CreateNewSlab() {
  auto* slab = MemorySlab::CreateSlab({
    object_size_,
    force_inline_header_,
    this
  });

  if (!slab) throw std::runtime_error("No slab created");

  return slab;
}

void MemoryCache::SwapCurrentSlabToFull() {
  current_slab_->NextSlab(slab_list_full_);

  slab_list_full_ = current_slab_;

  current_slab_ = nullptr;
}

void MemoryCache::SwapPartialSlabToCurrent() {
  if (current_slab_) throw std::runtime_error("Current slab is not empty");

  auto* partial = slab_list_partial_;

  if (!partial) throw std::runtime_error("No partial slab available");

  slab_list_partial_ = partial->NextSlab();
  current_slab_ = partial;
}

void MemoryCache::SwapSlabToPartial(MemorySlab* slab) {
  auto* curr_slab = slab_list_full_;
  MemorySlab* prev_slab = nullptr;

  while (curr_slab) {
    if (slab == curr_slab) {
      if (prev_slab) prev_slab->NextSlab( current_slab_->NextSlab() );
      else slab_list_full_ = curr_slab->NextSlab();

      curr_slab->NextSlab(slab_list_partial_);
      slab_list_partial_ = curr_slab;
    }
    prev_slab = curr_slab;
    curr_slab = curr_slab->NextSlab();
  }
}


void* MemoryCache::AllocateObject() {
  if (!current_slab_) current_slab_ = CreateNewSlab();

  auto* obj = current_slab_->GetFreeObject();

  // if slab can't give object - than there is no available objects in slab
  if (!obj) {
    SwapCurrentSlabToFull();

    if (slab_list_partial_)
      SwapPartialSlabToCurrent();
    else
      current_slab_ = CreateNewSlab();

    obj = current_slab_->GetFreeObject();
  }

  return obj;
}

bool MemoryCache::TryReturnObject(void* object_ptr) {
  auto* slab = SlabOfMemory(object_ptr);
  if ( !slab ) return false;

  slab->ReturnObject(object_ptr);

  SwapSlabToPartial(slab);
  return true;
}



size_t MemoryCache::MaxObjSize() const { return object_size_; }

MemorySlab* MemoryCache::SlabOfMemory(void* ptr) {
  // damn, I need to improve this, but I don't really know how to do it properly,
  // as I would need to have huge amount of metadata for each page of slab

  auto* curr_slap = current_slab_;
  while (curr_slap) {
    if ( curr_slap->IsPtrBelongsToSlab(ptr) ) return curr_slap;
  }

  curr_slap = slab_list_partial_;
  while (curr_slap) {
    if ( curr_slap->IsPtrBelongsToSlab(ptr) ) return curr_slap;
  }

  curr_slap = slab_list_full_;
  while (curr_slap) {
    if ( curr_slap->IsPtrBelongsToSlab(ptr) ) return curr_slap;
  }

  return nullptr;
}
