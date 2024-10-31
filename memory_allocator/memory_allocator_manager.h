#ifndef MEMORY_ALLOCATOR_MANAGER_H
#define MEMORY_ALLOCATOR_MANAGER_H

#include <gtest/gtest_prod.h>

#include "memory_arena.h"
#include "memory_block.h"


class AllocatorManager {
public:
    AllocatorManager()=delete;

    static void DefaultInitiateManager();
    static MemoryArena *DefaultAddArena(size_t size);
    
    static void ClearAll();

    static MemoryArena* CreateArena(size_t size);
    static void AppendArena(MemoryArena* arena);

    static bool IsInited();

    static MemoryArena* ArenaEntry();
    static MemoryBlock* BlockEntry();

    static MemoryBlock* FindFreeBlock(size_t userspace_size, MemoryBlock* starting_block = nullptr);
    static MemoryBlock* FindFreeBlockAnyArena(size_t size);
    static MemoryBlock* FindFreeBlockOrAppendArena(size_t size);

    static void OccupyBlock(MemoryBlock* block, size_t size);
    static void FreeBlock(MemoryBlock* block);

    static void TryMergeAdjacent(MemoryBlock* block);
    static bool TryMergeNext(MemoryBlock* block);
    static bool TryMergePrev(MemoryBlock* block);

protected:
    static bool IsBlockSuitable(MemoryBlock* block, size_t userspace_size, BLOCK_STATUS desired_status = BLOCK_STATUS::FREE);

    static MemoryArena* memory_arena_entry_;
    static MemoryBlock* memory_block_entry_;
};



#endif //MEMORY_ALLOCATOR_MANAGER_H
