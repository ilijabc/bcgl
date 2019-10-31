cmake_minimum_required(VERSION 3.6)

project({{bcapp:name}})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:bcgl_dir}} ${CMAKE_CURRENT_BINARY_DIR}/bcgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:src_dir}} ${CMAKE_CURRENT_BINARY_DIR}/src)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(WIN32 AND EXISTS "res/resource.rc")
    set(RES "res/resource.rc")
endif()

add_executable({{bcapp:name}} "version.c" ${RES})
target_link_libraries({{bcapp:name}} bcgl_lib src_lib)
