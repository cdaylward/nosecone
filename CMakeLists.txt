cmake_minimum_required(VERSION 2.8)
project(nosecone CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(3rdparty)

include_directories(src)
include_directories(${3RDPARTY_USR}/include)

#find_library(LIB_ARCHIVE NAMES archive PATHS ${3RDPARTY_USR}/lib)
set(LIB_ARCHIVE iconv lzma bz2 z xml2 ${3RDPARTY_USR}/lib/libarchive.a)

find_library(LIB_CURL NAMES curl PATHS ${3RDPARTY_USR}/lib)


file(GLOB NSCN_SOURCES src/nosecone/*.cpp)

add_executable(nscn ${NSCN_SOURCES})
target_link_libraries(nscn ${LIB_ARCHIVE} ${LIB_CURL})

