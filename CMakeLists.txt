cmake_minimum_required(VERSION 3.22.1)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.inl
)

# Collect all source files
file(GLOB_RECURSE COMPILATION_UNITS_TMP CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c++
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

# Filter out the test files
set(COMPILATION_UNITS "")
foreach(file ${COMPILATION_UNITS_TMP})
    if (NOT file MATCHES "(_test|main)\\.(cpp|c\\+\\+|cc)$")
        list(APPEND COMPILATION_UNITS ${file})
    endif()
endforeach()

file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.c++
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cс
)


add_executable(lab1 main.cpp ${COMPILATION_UNITS} ${HEADERS})
#
#set(GTESTLIBS GTest::gtest_main GTest::GTest GTest::Main)
#find_package(GTest REQUIRED)
#
#enable_testing()
#add_executable(tests-all ${TESTS} ${HEADERS} ${COMPILATION_UNITS} )
#
#target_link_libraries(tests-all ${GTESTLIBS})
#
#include(GoogleTest)
#gtest_discover_tests(tests-all)
