@echo off

mkdir debug
cd debug

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..\
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

mingw32-make
if not %ERRORLEVEL% == 0 (
    exit /B 2
)

cd ..
if "%1" == "run" (
    debug\src\bcgl.exe
)

exit /B 0
