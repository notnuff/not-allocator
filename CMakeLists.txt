cmake_minimum_required(VERSION 3.22.1)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})


file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
)

# Collect all source files
file(GLOB_RECURSE COMPILATION_UNITS_TMP CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c++
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

# Filter out the test files
set(COMPILATION_UNITS "")
foreach(file ${COMPILATION_UNITS_TMP})
    if (NOT file MATCHES "(test|main)\\.(cpp|c\\+\\+|cc)$")
        list(APPEND COMPILATION_UNITS ${file})
    endif()
endforeach()

file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*test.c++
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*test.cс
)

add_executable(allocators main.cpp ${HEADERS} ${TESTS} ${COMPILATION_UNITS} )

set(GTESTLIBS GTest::gtest_main GTest::GTest GTest::Main)
find_package(GTest REQUIRED)

enable_testing()
add_executable(tests-all ${TESTS} ${COMPILATION_UNITS} )

target_link_libraries(tests-all ${GTESTLIBS})
target_link_libraries(allocators ${GTESTLIBS})

include(GoogleTest)
gtest_discover_tests(tests-all)
