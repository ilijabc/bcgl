cmake_minimum_required(VERSION 3.6)

project(__bcgl_project_name__)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_BINARY_DIR}/src)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(WIN32 AND EXISTS "res/resource.rc")
    set(RES "res/resource.rc")
endif()

add_executable(__bcgl_project_name__ "version.c" ${RES})
target_link_libraries(__bcgl_project_name__ bcgl_lib src_lib)
