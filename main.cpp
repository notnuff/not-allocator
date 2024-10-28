#include <iostream>
#include "memory_allocator/allocator.h"

int main() {
  auto* address = (int*) mem_alloc(sizeof(int) * 15000);

  for(int i = 1; i < 10000; i++) {
    *(address + i) = i;
  }
  // std::cout << "Hello, World!" << std::endl;
  return 0;
}
