mkdir -p debug

(
    cd debug &&
    cmake -G 'MinGW Makefiles' -DCMAKE_SH='CMAKE_SH-NOTFOUND' .. &&
    mingw32-make
)
