#ifndef MEMORY_ALLOCATOR_MANAGER_H
#define MEMORY_ALLOCATOR_MANAGER_H

#include "memory_arena.h"
#include "memory_block.h"


class AllocatorManager {
public:
    AllocatorManager()=delete;

    static void DefaultInitiateManager();
    static void InitiateManager(void* memory_chunk, size_t memory_chunk_size);

    static MemoryArena* CreateArena(size_t size);
    static void AppendArena(MemoryArena* arena);

    static inline bool IsInited();

    static MemoryArena* ArenaEntry();
    static MemoryBlock* BlockEntry();

    static MemoryBlock* FindFreeBlock(size_t size, MemoryBlock* starting_block = nullptr);
    static void OccupyBlock(MemoryBlock* block, size_t size);
    static void FreeBlock(MemoryBlock* block);

protected:
    static inline bool IsBlockSuitable(MemoryBlock* block, size_t size, BLOCK_STATUS desired_status = BLOCK_STATUS::FREE);

    static MemoryArena* memory_arena_entry_;
    static MemoryBlock* memory_block_entry_;
};



#endif //MEMORY_ALLOCATOR_MANAGER_H
