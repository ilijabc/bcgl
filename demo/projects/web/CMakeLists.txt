cmake_minimum_required(VERSION 3.6)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/bcgl)

file(GLOB_RECURSE SRCS ../../src/*.c*)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../assets)

add_executable(test_app ${SRCS})

target_link_libraries(test_app bcgl)

set_target_properties(test_app PROPERTIES SUFFIX ".js" LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4 --preload-file ${ASSETS_DIR}@assets -O2")
em_link_js_library(test_app ${libraryJsFiles})
# add -O2 to minify
