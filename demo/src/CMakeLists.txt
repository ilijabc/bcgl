cmake_minimum_required(VERSION 3.6)

project(src_lib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SRCS *.c*)

include_directories(../../include ./)

add_library(src_lib STATIC ${SRCS})

target_link_libraries(src_lib)
