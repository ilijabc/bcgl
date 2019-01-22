@echo off

rem To call under Cygwin make sure CMake and Mingw Toolchain is in the PATH
rem Note that -DCMAKE_SH='CMAKE_SH-NOTFOUND' is necesarry to execute from Cygwin terminal

set BCGL_ROOT="%cd%"

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\win32"
)

echo BCGL_OUTPUT=%BCGL_OUTPUT%
echo BCGL_ROOT=%BCGL_ROOT%

mkdir %BCGL_OUTPUT%
cd %BCGL_OUTPUT%

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SH="CMAKE_SH-NOTFOUND" %BCGL_ROOT%
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

mingw32-make
if not %ERRORLEVEL% == 0 (
    exit /B 2
)

cd %BCGL_ROOT%

exit /B 0
