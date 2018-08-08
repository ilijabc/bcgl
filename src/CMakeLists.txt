cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

file(GLOB SRCS *.c)
set(LIBS external/libs.c)

# add_definitions("-D OPENGL_LEGACY")
# add_definitions("-D SUPPORT_GLSL")
# add_definitions("-D SUPPORT_GLEW")
add_definitions("-D SUPPORT_GLAD")
# add_definitions("-DGLEW_STATIC -DGLEW_NO_GLU")

add_definitions("-DBC_MAIN_LOOP")

add_executable(bcgl ${SRCS} ${LIBS})

target_include_directories(bcgl PUBLIC external external/glad/include)

target_link_libraries(bcgl glfw3 winmm gdi32 glu32 opengl32 wldap32 ws2_32 pthread)
