#include "allocator.h"

#include <cstdio>

#include "memory_arena.h"
#include "memory_block.h"
#include "memory_chunk.h"
#include "memory_allocator_manager.h"

constexpr auto DEFAULT_MEMORY_CHUNK_SIZE = DEFAULT_ARENA_SIZE * DEFAULT_PAGE_SIZE;

// my implementation idea - just to have one linked list of memory blocks,
// even if this blocks are in separate arenas


void *mem_alloc(size_t size) {
  if( !AllocatorManager::IsInited() ) {
    AllocatorManager::DefaultInitiateManager();
  }


  auto* found_block = AllocatorManager::FindFreeBlock(size);
  // if(!found_block) found_block = find_free_memory_block(memory_arena_entry, size);
  // if(!found_block) ;// add new memory block

  AllocatorManager::OccupyBlock(found_block, size);

  return found_block->UserSpace();
}

void mem_free(void *ptr) {
  // TODO: non-continuous memory blocks?
  // Possible solution - just check whether we can merge blocks by memory pointers

  auto* current_block = (MemoryBlock*) ( ptr - MemoryBlock::HeaderSize() );
  AllocatorManager::FreeBlock(current_block);

}

void *mem_realloc(void *ptr, size_t new_size) {
  return nullptr;
}

