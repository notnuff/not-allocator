#include <iostream>
#include <gtest/gtest.h>
#include <memory_allocators_apis/mem_allocator.h>

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
