cmake_minimum_required(VERSION 2.8.12)
project(persistent-rbtree)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL
  "Disable Google benchmark tests")
add_subdirectory(benchmark)

if(CMAKE_VERSION VERSION_LESS "3.8")
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
  if(NOT COMPILER_SUPPORTS_CXX17)
    message(FATAL_ERROR
      "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_executable(full_test test.cc)

add_executable(bench bench.cc)
target_link_libraries(bench benchmark)
