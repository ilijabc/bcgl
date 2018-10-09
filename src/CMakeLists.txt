cmake_minimum_required(VERSION 3.6)

set(BCGL_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# file(GLOB SRCS *.c)
if(ANDROID)
    list(APPEND SRCS bcgl_app_android.c)
else()
    list(APPEND SRCS bcgl_app_glfw.c)
    list(APPEND SRCS external/glad/src/glad.c)
endif()
list(APPEND SRCS bcgl_app_common.c)
list(APPEND SRCS bcgl_file.c)
list(APPEND SRCS bcgl_gfx.c)
list(APPEND SRCS bcgl_gfx_draw.c)
list(APPEND SRCS bcgl_gfx_font.c)
list(APPEND SRCS bcgl_gfx_geometry.c)
list(APPEND SRCS bcgl_gfx_model.c)
list(APPEND SRCS bcmath.c)

add_definitions("-D SUPPORT_GLSL")
add_definitions("-D SUPPORT_PAR_SHAPES")
add_definitions("-D SUPPORT_GLAD")

add_library(bcgl ${SRCS})

target_include_directories(bcgl PUBLIC include external external/glad/include)

if(ANDROID)
    include_directories(${ANDROID_SYSROOT}/usr/include/arm-linux-androideabi)
endif()

if(WIN32)
    set(LIBS glfw3 winmm gdi32 glu32 opengl32 wldap32 ws2_32 pthread)
elseif(ANDROID)
    set(LIBS OpenSLES EGL GLESv2 android log m)
else()
    set(LIBS glfw GL m dl pthread)
endif()

target_link_libraries(bcgl ${LIBS})
