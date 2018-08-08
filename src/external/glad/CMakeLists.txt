cmake_minimum_required(VERSION 3.12)

file(GLOB SRCS src/*.c)

add_library(glad ${SRCS})

target_include_directories(glad PUBLIC
    ./include
    )

# target_link_libraries(bcgl glfw glew winmm gdi32 glu32 opengl32 wldap32 ws2_32 pthread)
