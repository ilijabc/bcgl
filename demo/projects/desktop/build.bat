@echo off

rem To call under Cygwin make sure CMake and Mingw Toolchain is in the PATH
rem Note that -DCMAKE_SH='CMAKE_SH-NOTFOUND' is necesarry to execute from Cygwin terminal

mkdir %~dp0\build
pushd %~dp0\build

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SH="CMAKE_SH-NOTFOUND" ..
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

mingw32-make
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 2
)

popd
exit /B 0
