cmake_minimum_required(VERSION 2.8)
project(nosecone CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

file(GLOB NSCN_SOURCES src/nosecone/*.cpp)
add_executable(nscn ${NSCN_SOURCES})

