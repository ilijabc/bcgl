cmake_minimum_required(VERSION 3.6)

project(example_lib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SRCS *.c*)

include_directories(./)

add_library(example_lib STATIC ${SRCS})

target_link_libraries(example_lib)
