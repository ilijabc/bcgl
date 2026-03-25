cmake_minimum_required(VERSION 3.6)

project(__bcgl_project_name__)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../src ${CMAKE_CURRENT_BINARY_DIR}/src)

add_library(native-lib SHARED "version.c")
target_link_libraries(native-lib bcgl_lib src_lib)
