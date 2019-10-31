cmake_minimum_required(VERSION 3.6)

project(demo)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/bcgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_BINARY_DIR}/src)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(WIN32 AND EXISTS "res/resource.rc")
    set(RES "res/resource.rc")
endif()

add_executable(demo "version.c" ${RES})
target_link_libraries(demo bcgl_lib src_lib)
