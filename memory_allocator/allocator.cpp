#include "allocator.h"

#include "memory_arena.h"
#include "memory_block.h"
#include "memory_chunk.h"

static memory_arena_t* memory_arena_entry = nullptr;
static memory_block_t* memory_block_entry = nullptr;

constexpr auto DEFAULT_MEMORY_CHUNK_SIZE = DEFAULT_ARENA_SIZE * DEFAULT_PAGE_SIZE;


// my implementation idea - just to have one linked list of memory blocks,
// even if this blocks are in separate arenas

void *mem_alloc(size_t size) {
  if(!memory_arena_entry) {
    void* arena_raw_ptr = get_new_memory_chunk(DEFAULT_MEMORY_CHUNK_SIZE);
    memory_arena_entry = (memory_arena_t*) arena_raw_ptr;
    memory_arena_entry->size = DEFAULT_MEMORY_CHUNK_SIZE;
    memory_arena_entry->next = NULL;

    memory_block_entry = (memory_block_t*) (memory_arena_entry + MEMORY_ARENA_HEADER_SIZE);

    memory_block_entry->size = memory_block_entry->size - MEMORY_ARENA_HEADER_SIZE;
    memory_block_entry->prev_block_begin = NULL;
    memory_block_entry->next_block_begin = NULL;

  }





  return nullptr;
}

void mem_free(void *ptr) {
  // TODO: non-continuous memory blocks?
  // Possible solution - just check whether we can merge blocks by memory pointers

  auto* current_block = (memory_block_t*) (ptr - MEMORY_BLOCK_HEADER_SIZE);
  current_block->tag = BLOCK_STATUS::FREE;
  if(auto next = current_block->prev_block_begin) {
    if(next->tag == BLOCK_STATUS::FREE) {
      current_block->size += next->size;
      current_block->next_block_begin = next->next_block_begin;
    }
  }
  if(auto prev = current_block->prev_block_begin) {
    if(prev->tag == BLOCK_STATUS::FREE) {
      prev->size += current_block->size;
      prev->next_block_begin = current_block->next_block_begin;
    }
  }
}

void *mem_realloc(void *ptr, size_t new_size) {
  return nullptr;
}
