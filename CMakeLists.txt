cmake_minimum_required(VERSION 3.6)

add_subdirectory(src)

if ($ENV{BCGL_DEMO})
    add_subdirectory(demo)
endif()
