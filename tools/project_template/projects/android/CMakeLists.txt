cmake_minimum_required(VERSION 3.6)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:bcgl_dir}} ${CMAKE_CURRENT_BINARY_DIR}/bcgl)

file(GLOB_RECURSE SRCS {{bcapp:src_dir}}/*.c*)

add_library(native-lib SHARED ${SRCS})

find_library(log-lib log)

find_library(android-lib android)

target_link_libraries(native-lib ${log-lib} ${android-lib} bcgl)
