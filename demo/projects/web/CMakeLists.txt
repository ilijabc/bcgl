cmake_minimum_required(VERSION 3.6)

project(demo)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/bcgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_BINARY_DIR}/src)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../assets)

add_executable(demo "version.c")
target_link_libraries(demo bcgl_lib src_lib)
set_target_properties(demo PROPERTIES SUFFIX ".js" LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4 --preload-file ${ASSETS_DIR}@assets -O2")
em_link_js_library(demo ${libraryJsFiles})
# add -O2 to minify
