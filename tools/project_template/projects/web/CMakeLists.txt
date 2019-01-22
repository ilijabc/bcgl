cmake_minimum_required(VERSION 3.6)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:bcgl_dir}} ${CMAKE_CURRENT_BINARY_DIR}/bcgl)

file(GLOB_RECURSE SRCS {{bcapp:src_dir}}/*.c*)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:assets_dir}})

add_executable({{bcapp:name}} ${SRCS})

target_link_libraries({{bcapp:name}} bcgl)

set_target_properties({{bcapp:name}} PROPERTIES SUFFIX ".html" LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4 --preload-file ${ASSETS_DIR}@assets -O2")
em_link_js_library({{bcapp:name}} ${libraryJsFiles})
# add -O2 to minify
