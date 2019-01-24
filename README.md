# BCGL

This lib (let's call it that way) was privately used for in-house projects.
I decided to publish it so maybe it is not convenient to use since nothing
is documented. This is a work in progress...

## Init a new project

Do following to initialize a new project based on BCGL:

```
git clone https://github.com/ilijabc/bcgl.git
./bcgl/tools/create_project.py -C my.company -N myapp -T "My App"
```

This project is inspired by following projects:
- raylib - https://github.com/raysan5/raylib
- mathc - https://github.com/felselva/mathc
- libGDX - https://github.com/libgdx/libgdx

## Dependencies

Libraries:
- STB - https://github.com/nothings/stb
- GLFW - https://github.com/glfw/glfw
- GLAD - https://github.com/Dav1dde/glad
- PAR - https://github.com/prideout/par

Build tools:
- CMake - https://cmake.org/
- Mingw-w64 - https://mingw-w64.org/doku.php
- Emscripten - https://kripken.github.io/emscripten-site/
- Android SDK/NDK - https://developer.android.com/
- Python - https://www.python.org/
