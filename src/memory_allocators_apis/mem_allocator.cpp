#include "mem_allocator.h"

#include <general_memory_abstractions/memory_chunk.h>
#include <slob_allocator/memory_allocator_manager.h>
#include <slob_allocator/memory_arena.h>
#include <slob_allocator/memory_block.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>


constexpr auto DEFAULT_MEMORY_CHUNK_SIZE = DEFAULT_ARENA_SIZE * DEFAULT_PAGE_SIZE;

// my implementation idea - just to have one linked list of memory blocks,
// even if this blocks are in separate arenas


void *mem_alloc(size_t size) {
  if( !AllocatorManager::IsInited() ) {
    AllocatorManager::DefaultInitiateManager();
  }

  auto* found_block = AllocatorManager::FindFreeBlock(size);
  if(!found_block) {
    auto full_arena_size = size + MemoryBlock::HeaderSize() + MemoryArena::HeaderSize();
    auto* new_arena = AllocatorManager::DefaultAddArena(full_arena_size);
    auto* new_block_start = (MemoryBlock*) new_arena->UserSpace();


    found_block = AllocatorManager::FindFreeBlock(size, new_block_start);
  }

  if(!found_block) {
    throw std::bad_alloc();
  }

  AllocatorManager::OccupyBlock(found_block, size);

  return found_block->UserSpace();
}

void mem_free(void *ptr) {
  // TODO: non-continuous memory blocks?
  // Possible solution - just check whether we can merge blocks by memory pointers

  auto* current_block = (MemoryBlock*) ( (char*) ptr - MemoryBlock::HeaderSize() );
  AllocatorManager::FreeBlock(current_block);

}

void *mem_realloc(void *ptr, size_t new_size) {
  auto* current_block = (MemoryBlock*) ( (char*) ptr - MemoryBlock::HeaderSize() );

  if( current_block->UserSpaceSize() < new_size ) AllocatorManager::TryMergeNext(current_block);
  if( current_block->UserSpaceSize() >= new_size ) return current_block->UserSpace();

  auto* found_ptr = mem_alloc(new_size);
  memcpy( found_ptr, current_block->UserSpace(), std::min(current_block->UserSpaceSize(), new_size) );

  mem_free(ptr);

  return found_ptr;
}

