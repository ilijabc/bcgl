cmake_minimum_required(VERSION 3.6)

add_subdirectory(src)

if (BCGL_DEMO)
    add_subdirectory(demo)
endif()
