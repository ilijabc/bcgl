cmake_minimum_required(VERSION 3.8)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

file(GLOB SRCS *.c)

add_executable(example ${SRCS})

target_include_directories(example PUBLIC ../)

target_link_libraries(example bcgl)
