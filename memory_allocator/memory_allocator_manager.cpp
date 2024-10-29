#include "memory_allocator_manager.h"

#include <cstring>
#include <new>

#include "memory_chunk.h"

constexpr auto DEFAULT_MEMORY_CHUNK_SIZE = DEFAULT_ARENA_SIZE * DEFAULT_PAGE_SIZE;


void AllocatorManager::DefaultInitiateManager() {
    auto* arena = CreateArena(DEFAULT_MEMORY_CHUNK_SIZE);
    AppendArena(arena);

    if(!memory_arena_entry_) throw std::bad_alloc();

    memory_block_entry_ =
        MemoryBlock::ConvertToMemoryBlock( memory_arena_entry_->UserSpace(), memory_arena_entry_->UserSpaceSize() );
    if(!memory_block_entry_) throw std::bad_alloc();
}

MemoryArena* AllocatorManager::CreateArena(size_t size) {
    MemoryArena* ptr_to_arena = nullptr;

    void* raw_chunk = MemoryChunk::AppendNewMemoryChunk(size);
    ptr_to_arena = MemoryArena::ConvertToMemoryArena(raw_chunk, size);

    return ptr_to_arena;
}

void AllocatorManager::AppendArena(MemoryArena *arena) {
    if(memory_arena_entry_ == nullptr) {
        memory_arena_entry_ = arena;
        return;
    }

    auto* current_arena = memory_arena_entry_;
    while(current_arena->Next() != nullptr) {
        current_arena = current_arena->Next();
    }

    current_arena->Next(arena);
}


bool AllocatorManager::IsInited() {
    return memory_arena_entry_;
}

MemoryArena* AllocatorManager::ArenaEntry() {
    return memory_arena_entry_;
}

MemoryBlock* AllocatorManager::BlockEntry() {
    return memory_block_entry_;

}

MemoryBlock* AllocatorManager::FindFreeBlock(size_t size, MemoryBlock* starting_block) {
    auto* current_lookup_block = starting_block;

    if(current_lookup_block == nullptr) {
        current_lookup_block = (MemoryBlock*) ArenaEntry()->UserSpace();
    }

    while(current_lookup_block) {
        if( IsBlockSuitable(current_lookup_block, size) ) {
            return current_lookup_block;
        }
        current_lookup_block = current_lookup_block->Next();
    }

    return nullptr;
}

void AllocatorManager::OccupyBlock(MemoryBlock* block, size_t size) {
    if(block == nullptr) throw std::bad_alloc();

    auto space_remaining = block->Size() - size;
    if( space_remaining > MemoryBlock::Size() ) {

        // I TOTALLY HATE THIS POINTER ARITHMETICS
        auto* new_block = (MemoryBlock*) ((char*) block->UserSpace() + size);

        new_block->Size(space_remaining);
        new_block->Prev(block);
        new_block->Next( block->Next() );

        block->Next(new_block);
    }

    block->Tag(BLOCK_STATUS::OCCUPIED);
}

void AllocatorManager::FreeBlock(MemoryBlock* block) {
    block->Tag(BLOCK_STATUS::FREE);
    // try merge blocks

    if( auto next = block->Next() ) {
        if(next->Tag() == BLOCK_STATUS::FREE) {
            block->Size( block->Size() + next->Size() );
            block->Next( next->Next() );

            memset( next, 0, MemoryBlock::HeaderSize() );
        }
    }

    if( auto prev = block->Prev() ) {
        if(prev->Tag() == BLOCK_STATUS::FREE) {
            prev->Size( prev->Size() + block->Size() );
            prev->Next( block->Next() );
        }
    }
}

bool AllocatorManager::IsBlockSuitable(MemoryBlock *block, size_t size, BLOCK_STATUS desired_status) {
    bool result =
      block->Tag() == BLOCK_STATUS::FREE &&
      block->UserSpaceSize()>= size;
    return result;
}
