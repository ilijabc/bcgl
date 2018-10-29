cmake_minimum_required(VERSION 3.6)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

file(GLOB SRCS *.c)
if(WIN32)
    # set(RES "res/resource.rc")
endif()

add_executable(bcgl-demo ${SRCS} ${RES})

target_include_directories(bcgl-demo PUBLIC ${BCGL_ROOT}/include)

target_link_libraries(bcgl-demo bcgl)

if(EMSCRIPTEN)
    set_target_properties(bcgl-demo PROPERTIES SUFFIX ".html" LINK_FLAGS " --bind -s  NO_EXIT_RUNTIME=1 -g4")
    em_link_js_library(bcgl-demo ${libraryJsFiles})
    # add -O2 to minify
endif(EMSCRIPTEN)
