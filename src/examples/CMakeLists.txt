cmake_minimum_required(VERSION 3.6)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

file(GLOB SRCS *.c)


if(ANDROID)
    include_directories(${ANDROID_SYSROOT}/usr/include/arm-linux-androideabi)
    add_library(example ${SRCS})
else()
    add_executable(example ${SRCS})
endif()

target_include_directories(example PUBLIC ../)

target_link_libraries(example bcgl)
