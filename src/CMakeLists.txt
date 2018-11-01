cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_FLAGS "-std=c99")

set(BCGL_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions("-D SUPPORT_PAR_SHAPES")

if(ANDROID)
    # message("Android platform")
    add_definitions(-D SUPPORT_GLES)
    add_definitions(-D SUPPORT_GLSL)
    include_directories(${ANDROID_SYSROOT}/usr/include/arm-linux-androideabi)
    set(LIBS EGL GLESv2 android log m)
elseif(EMSCRIPTEN)
    # message("Web platofrm")
    add_definitions(-D SUPPORT_GLES)
    add_definitions(-D SUPPORT_GLSL)
    set(LIBS GL m dl pthread)
elseif(WIN32)
    # message("Win32 platform")
    add_definitions(-D SUPPORT_GLAD)
    add_definitions(-D SUPPORT_GLSL)
    set(LIBS glfw3 winmm gdi32 glu32 opengl32 wldap32 ws2_32 pthread)
elseif(UNIX)
    # message("Unix platform")
    add_definitions(-D SUPPORT_GLAD)
    add_definitions(-D SUPPORT_GLSL)
    set(LIBS glfw GL m dl pthread)
else()
    message(FATAL_ERROR "Platform not supported!")
endif()

file(GLOB SRCS *.c)

if(ANDROID)
    list(APPEND SRCS port/bcgl_app_android.c)
elseif(EMSCRIPTEN)
    list(APPEND SRCS port/bcgl_app_web.c)
else()
    list(APPEND SRCS port/bcgl_app_glfw.c)
    list(APPEND SRCS ../external/glad/src/glad.c)
endif()

add_library(bcgl ${SRCS})

target_include_directories(bcgl PUBLIC ../include ../external ../external/glad/include)

target_link_libraries(bcgl ${LIBS})
