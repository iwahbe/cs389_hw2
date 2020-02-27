cmake_minimum_required(VERSION 3.16.0)

SET(DCMAKE_BUILD_TYPE Debug)

SET(DCMAKE_EXPORT_COMPILE_COMMANDS YES)

SET(CMAKE_CXX_COMPILER "g++")

project(hash_it_out CXX)

SET(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -std=c++17 -stdlib=libc++")

add_executable(test test.cc cache_lib.cc evictor_lib.cc cache.hh evictor.hh)

