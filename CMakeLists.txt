cmake_minimum_required(VERSION 3.6)

add_subdirectory(src)

if (NOT ANDROID)
    add_subdirectory(demo)
endif()
