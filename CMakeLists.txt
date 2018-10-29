cmake_minimum_required(VERSION 3.6)

add_subdirectory(src)

if (WIN32 OR UNIX)
    add_subdirectory(demo)
endif()
