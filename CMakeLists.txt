cmake_minimum_required(VERSION 3.16.0)

SET(DCMAKE_BUILD_TYPE Debug)

SET(DCMAKE_EXPORT_COMPILE_COMMANDS YES)

SET(CMAKE_CXX_COMPILER "g++")

project(hash_it_out CXX)

add_library(cache STATIC cache_lib.cc cache.hh evictor.hh)

add_library(fifo_evictor STATIC evictor.hh fifo_evictor.hh fifo_evictor.cc)

SET(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -std=c++17 -stdlib=libc++")



add_executable(test test.cc)
target_link_libraries(test PUBLIC cache)
target_link_libraries(test PUBLIC fifo_evictor)
