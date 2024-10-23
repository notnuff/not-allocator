#include <cstddef>
#include <sys/mman.h>

#include "memory_chunk.h"


// so, through various experiments I figured out,
// that when user allocates < ~60k bytes of memory,
// malloc appends 15 pages of size 4k
// then, the stepping is like that:
// 48 pages
// 53 pages
// 58 pages
// ...
// so, I probably will copy that logic

void* get_new_memory_chunk(size_t bytes) {
  void* new_map_address = mmap(nullptr, bytes,
                               PROT_EXEC | PROT_READ | PROT_WRITE | PROT_NONE,
                               MAP_PRIVATE | MAP_ANON,
                               -1, 0);
  return new_map_address;
}

void remove_arena(void *ptr) {

}
