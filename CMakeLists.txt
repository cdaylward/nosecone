cmake_minimum_required(VERSION 2.8)
project(nosecone CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(3rdparty)

include_directories(src)
include_directories(${3RDPARTY_USR}/include)

find_library(LIB_CRYPTO NAMES crypto)
find_library(LIB_LZMA NAMES lzma)
find_library(LIB_BZ2 NAMES bz2)
find_library(LIB_Z NAMES z)
find_library(LIB_CURL NAMES curl PATHS ${3RDPARTY_USR}/lib)
find_library(LIB_ARCHIVE NAMES archive PATHS ${3RDPARTY_USR}/lib)

file(GLOB NSCN_SOURCES
  src/nosecone/executor/container/*.cpp
  src/nosecone/executor/*.cpp
  src/nosecone/command/*.cpp
  src/nosecone/*.cpp)

add_executable(nscn ${NSCN_SOURCES})
target_link_libraries(nscn ${LIB_ARCHIVE} ${LIB_CURL} ${LIB_CRYPTO} ${LIB_LZMA} ${LIB_BZ2} ${LIB_Z})

add_dependencies(nscn _get_libarchive _get_libcurl)
