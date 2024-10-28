#include "memory_block.h"

MemoryBlock* MemoryBlock::ConvertToMemoryBlock(void *start, size_t size) {
    if(sizeof(MemoryBlock) > size) return nullptr;

    auto* block = (MemoryBlock*) start;
    block->size_ = size;
    block->next_block_begin = nullptr;
    block->prev_block_begin = nullptr;
    block->tag = BLOCK_STATUS::FREE;

}

size_t MemoryBlock::Size() {
    return size_;
}

void MemoryBlock::Size(const size_t size) {
    size_ = size;
}

MemoryBlock* MemoryBlock::Next() {
    return next_block_begin;
}

void MemoryBlock::Next(MemoryBlock* next) {
    next_block_begin = next;
}

MemoryBlock* MemoryBlock::Prev() {
    return prev_block_begin;
}

void MemoryBlock::Prev(MemoryBlock* prev) {
    prev_block_begin = prev;
}

BLOCK_STATUS MemoryBlock::Tag() {
    return tag;
}

void MemoryBlock::Tag(BLOCK_STATUS block_status) {
    tag = block_status;
}

void* MemoryBlock::UserSpace() {
    return (char*) this + sizeof(MemoryBlock);
}

size_t MemoryBlock::UserSpaceSize() {
    return size_ - sizeof(MemoryBlock);
}

size_t MemoryBlock::HeaderSize() {
    return sizeof(MemoryBlock);
}
