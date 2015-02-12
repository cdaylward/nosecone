cmake_minimum_required(VERSION 2.8)
project(nosecone CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(3rdparty)

include_directories(src)
include_directories(${3RDPARTY_USR}/include)

#find_library(LIB_ARCHIVE NAMES archive
#  NO_DEFAULT_PATH
#  NO_CMAKE_ENVIRONMENT_PATH
#  NO_CMAKE_PATH
#  NO_SYSTEM_ENVIRONMENT_PATH
#  NO_CMAKE_SYSTEM_PATH
#  NO_CMAKE_FIND_ROOT_PATH
#  PATHS ${3RDPARTY_USR}/lib)
set(LIB_ARCHIVE lzma bz2 z xml2 ${3RDPARTY_USR}/lib/libarchive.a)

find_library(LIB_CRYPTO NAMES crypto)
find_library(LIB_CURL NAMES curl PATHS ${3RDPARTY_USR}/lib)


file(GLOB NSCN_SOURCES
  src/nosecone/executor/container/*.cpp
  src/nosecone/executor/*.cpp
  src/nosecone/command/*.cpp
  src/nosecone/*.cpp)

add_executable(nscn ${NSCN_SOURCES})
target_link_libraries(nscn ${LIB_ARCHIVE} ${LIB_CURL} ${LIB_CRYPTO})

