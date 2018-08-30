cmake_minimum_required(VERSION 3.8)

file(GLOB SRCS *.c)

add_definitions("-D SUPPORT_GLSL")
add_definitions("-D SUPPORT_PAR_SHAPES")
add_definitions("-D SUPPORT_GLAD")
add_definitions("-D BC_MAIN_LOOP")

add_library(bcgl ${SRCS})

target_include_directories(bcgl PUBLIC external external/glad/include)

if(WIN32)
    set(LIBS glfw3 winmm gdi32 glu32 opengl32 wldap32 ws2_32 pthread)
else()
    set(LIBS glfw GL m dl pthread)
endif()

target_link_libraries(bcgl ${LIBS})
