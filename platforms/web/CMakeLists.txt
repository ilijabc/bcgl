cmake_minimum_required(VERSION 3.6)

project(__bcgl_project_name__)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_BINARY_DIR}/src)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../assets)

add_executable(__bcgl_project_name__ "version.c")
target_link_libraries(__bcgl_project_name__ ${BCGL_TARGET_LIBS})
set_target_properties(__bcgl_project_name__ PROPERTIES SUFFIX ".js" LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4 --preload-file ${ASSETS_DIR}@assets -O2")
em_link_js_library(__bcgl_project_name__ ${libraryJsFiles})
# add -O2 to minify
