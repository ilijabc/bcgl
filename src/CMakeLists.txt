include_directories(bcgl/include)

add_subdirectory(bcgl)
add_subdirectory(example)

# set all libs to link to the final target
set(BCGL_TARGET_LIBS
    bcgl_lib
    example_lib
    PARENT_SCOPE)
