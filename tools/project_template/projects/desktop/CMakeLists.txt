cmake_minimum_required(VERSION 3.6)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/{{bcapp:bcgl_dir}} ${CMAKE_CURRENT_BINARY_DIR}/bcgl)

file(GLOB_RECURSE SRCS {{bcapp:src_dir}}/*.c*)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# if(WIN32)
#     set(RES "res/resource.rc")
# endif()

add_executable({{bcapp:name}} ${SRCS} ${RES})

target_link_libraries({{bcapp:name}} bcgl)
