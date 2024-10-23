#include <iostream>
#include "memory_allocator/allocator.h"

int main() {
  auto a = mem_alloc(5);
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
