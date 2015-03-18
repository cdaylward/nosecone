cmake_minimum_required(VERSION 2.8)
project(nosecone CXX)

set(GNUCXX_MINIMUM_VERSION "4.9")

if (CMAKE_COMPILER_IS_GNUCXX AND
    CMAKE_CXX_COMPILER_VERSION VERSION_LESS GNUCXX_MINIMUM_VERSION)
  message(FATAL_ERROR "Nose Cone requires GCC version >= ${GNUCXX_MINIMUM_VERSION}")
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic ")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3 -flto ")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

find_library(LIB_CRYPTO NAMES crypto)
find_library(LIB_LZMA NAMES lzma)
find_library(LIB_BZ2 NAMES bz2)
find_library(LIB_Z NAMES z)
find_library(LIB_CURL NAMES curl)
find_library(LIB_ARCHIVE NAMES archive)

file(GLOB NSCN_SOURCES
  src/nosecone/executor/container/*.cpp
  src/nosecone/executor/*.cpp
  src/nosecone/command/*.cpp
  src/nosecone/*.cpp)

add_executable(nscn ${NSCN_SOURCES})
target_link_libraries(nscn ${LIB_ARCHIVE} ${LIB_CURL} ${LIB_CRYPTO} ${LIB_LZMA} ${LIB_BZ2} ${LIB_Z})

