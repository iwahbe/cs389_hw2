cmake_minimum_required(VERSION 3.16.0)

SET(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -g -fsanitize=address")

add_library(cache STATIC cache_lib.cc cache.hh evictor.hh)

add_library(evictor STATIC evictor.hh fifo_evictor.hh
  fifo_evictor.cc lru_evictor.hh lru_evictor.cc)

set(CMAKE_CXX_COMPILER "clang++")


SET(CMAKE_BUILD_TYPE "Debug")

project(hash_it_out CXX)


enable_testing()
find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

add_executable(catch test.cc)
target_link_libraries(catch Catch2::Catch2 test_cache test_evictors)
catch_discover_tests(catch)

# basic test of raw cache
add_library(test_cache test_cache.cc)
target_link_libraries(test_cache PUBLIC cache evictor Catch2::Catch2)

# test of evictors
add_library(test_evictors test_evictors.cc)
target_link_libraries(test_evictors PUBLIC cache evictor Catch2::Catch2)

# setup tests to run on build
add_custom_target(MainTest ALL)
add_dependencies(MainTest catch)
add_custom_command(TARGET MainTest
  POST_BUILD
  COMMAND ctest --output-on-failure)
