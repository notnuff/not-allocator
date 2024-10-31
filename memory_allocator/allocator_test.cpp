#include <gtest/gtest.h>
#include <random>
#include <ranges>

#include "allocator.h"


#include "memory_allocator_manager.h"

int rand_num(int lower, int upper) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(lower, upper);
  return dis(gen);
}

void random_fill(void* ptr, size_t size) {
  auto* byte_ptr = (char*) ptr; // to make pointer arithmetics work fine

  for(auto* byte_ptr_end = byte_ptr + size; byte_ptr != byte_ptr_end; ++byte_ptr) {
    *byte_ptr = (char) rand_num(0, 255);
  }
}

int get_hash(void* ptr, size_t size) {
  int result = 0;

  auto* byte_ptr = static_cast<unsigned char*>(ptr);

  for(auto* byte_ptr_end = byte_ptr + size; byte_ptr != byte_ptr_end; ++byte_ptr) {
    result = (result * 31) ^ *byte_ptr;  // Multiply by a prime number and XOR with byte
  }

  return result;
}

constexpr auto min_alloc_size = 1;
constexpr auto max_alloc_size = 3301*4;

struct BlockRecord {
  MemoryArena* arena = nullptr;
  MemoryArena* next_arena = nullptr;
  size_t arena_size = 0;

  MemoryBlock* block_ptr = nullptr;
  MemoryBlock* next_ptr = nullptr;
  MemoryBlock* prev_ptr = nullptr;

  size_t block_size = 0;
  BLOCK_STATUS block_status = BLOCK_STATUS::FREE;

  size_t hash_size = 0;
  int block_inner_hash = 0;
} typedef BlockRecord;

BlockRecord create_block_record() {
  auto test_allocated_memory_size = rand_num(min_alloc_size, max_alloc_size);
  auto test_allocated_memory = mem_alloc(test_allocated_memory_size);

  if(!test_allocated_memory) return BlockRecord{};
  random_fill(test_allocated_memory, test_allocated_memory_size);

  auto* block = (MemoryBlock*) ( (char*) test_allocated_memory - MemoryBlock::HeaderSize() );

  BlockRecord record;
  record.block_ptr = block;
  record.prev_ptr = block->Prev();
  record.next_ptr = block->Next();
  record.block_size = record.block_ptr->Size();
  record.block_status = record.block_ptr->Tag();

  record.hash_size = test_allocated_memory_size;
  record.block_inner_hash = get_hash(test_allocated_memory, test_allocated_memory_size);

  return record;
}

void test_check_record_blocks_correctness(const std::vector<BlockRecord>& records) {
  for(const auto& record : records) {
    auto* block = record.block_ptr;
    EXPECT_TRUE(block->Size() > 0);
    EXPECT_EQ(block->Size(), record.block_size);
    EXPECT_EQ(block->Tag(), record.block_status);

    auto hash = get_hash( block->UserSpace(), record.hash_size );
    EXPECT_EQ(hash, record.block_inner_hash);
  }
}

void test_check_record_arenas_correctness(const std::vector<BlockRecord>& records) {
  for(const auto& record : records) {
    auto* arena = record.arena;
    EXPECT_EQ(arena->Size(), record.arena_size);
    EXPECT_EQ(arena->Next(), record.next_arena);
  }
}

TEST(AllocatorTest, AllocatorManagerInit) {
  EXPECT_EQ(AllocatorManager::ArenaEntry(), nullptr);
  EXPECT_EQ(AllocatorManager::BlockEntry(), nullptr);

  auto* mem = mem_alloc(1);

  EXPECT_NE(AllocatorManager::ArenaEntry(), nullptr);
  EXPECT_NE(AllocatorManager::BlockEntry(), nullptr);

  mem_free(mem);

  AllocatorManager::ClearAll();
}

constexpr auto min_arena_size = 2500;
constexpr auto max_arena_size = 18000;

TEST(AllocatorTest, TestAppendArenas) {

  AllocatorManager::DefaultInitiateManager();

  ASSERT_NE(AllocatorManager::ArenaEntry(), nullptr);

  // appending 100 arenas with random size
  std::vector<BlockRecord> records;
  for(int i = 0; i < 10000; ++i) {
    BlockRecord record;
    record.arena = AllocatorManager::DefaultAddArena( rand_num(min_arena_size, max_arena_size) );
    record.arena_size = record.arena->Size();
    records.push_back(record);

    if(i > 0) records[i-1].next_arena = record.arena;
  }

  test_check_record_arenas_correctness(records);

  AllocatorManager::ClearAll();
}


TEST(AllocatorTest, RandomAllocations) {
  std::vector<BlockRecord> records;

  auto* current_arena = AllocatorManager::ArenaEntry();
  if(!current_arena) {
    AllocatorManager::DefaultInitiateManager();
    current_arena = AllocatorManager::ArenaEntry();
  }

  ASSERT_NE(current_arena, nullptr);

  // just want to create 100 arenas, why not :)
  for(int i = 0; i < 100; ++i) {
    while( !current_arena->Next() ) {
      records.push_back( create_block_record() );
      test_check_record_blocks_correctness(records);
    }
    current_arena = current_arena->Next();
  }

  AllocatorManager::ClearAll();
}

TEST(AllocatorTest, RandomFree) {
  std::vector<BlockRecord> records;

  auto* current_arena = AllocatorManager::ArenaEntry();
  if(!current_arena) {
    AllocatorManager::DefaultInitiateManager();
    current_arena = AllocatorManager::ArenaEntry();
  }

  ASSERT_NE(current_arena, nullptr);

  for(int i = 0; i < 10; ++i) {
    while( !current_arena->Next() ) {
      records.push_back( create_block_record() );
    }
    current_arena = current_arena->Next();
  }

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle( records.begin(), records.end(), g);

  for (auto it = records.begin(); it != records.end(); ) {
    mem_free(it->block_ptr->UserSpace());
    it = records.erase(it);

    test_check_record_blocks_correctness(records);
  }


  AllocatorManager::ClearAll();
}


TEST(AllocatorTest, RandomReallocks) {
  std::vector<BlockRecord> records;

  auto* current_arena = AllocatorManager::ArenaEntry();
  if(!current_arena) {
    AllocatorManager::DefaultInitiateManager();
    current_arena = AllocatorManager::ArenaEntry();
  }

  ASSERT_NE(current_arena, nullptr);

  for(int i = 0; i < 10; ++i) {
    while( !current_arena->Next() ) {
      records.push_back( create_block_record() );
      test_check_record_blocks_correctness(records);
    }
    current_arena = current_arena->Next();
  }

  for(int i = 0; i < 10; ++i) {
    while( !current_arena->Next() ) {
      records.push_back( create_block_record() );
    }
    current_arena = current_arena->Next();
  }

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle( records.begin(), records.end(), g);

  for(auto& record : records) {
    auto newRandomSize = record.block_ptr->UserSpaceSize() + rand_num(min_alloc_size, max_alloc_size);

    record.block_ptr =
      (MemoryBlock*) ((char*) mem_realloc(record.block_ptr->UserSpace(), newRandomSize) - MemoryBlock::HeaderSize() );

    record.block_size = record.block_ptr->Size();
    record.prev_ptr = record.block_ptr->Prev();
    record.next_ptr = record.block_ptr->Next();

    test_check_record_blocks_correctness(records);
  }


  AllocatorManager::ClearAll();
}

