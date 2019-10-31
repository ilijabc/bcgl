cmake_minimum_required(VERSION 3.6)

project({{bcapp:name}})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:bcgl_dir}} ${CMAKE_CURRENT_BINARY_DIR}/bcgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:src_dir}} ${CMAKE_CURRENT_BINARY_DIR}/src)

add_library(native-lib SHARED "version.c")
target_link_libraries(native-lib bcgl_lib src_lib)
