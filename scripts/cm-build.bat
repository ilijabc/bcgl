@echo off

set BCGL_ROOT="%cd%"

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_debug"
)

echo BCGL_OUTPUT=%BCGL_OUTPUT%
echo BCGL_ROOT=%BCGL_ROOT%

mkdir %BCGL_OUTPUT%
cd %BCGL_OUTPUT%

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug %BCGL_ROOT%
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

mingw32-make
if not %ERRORLEVEL% == 0 (
    exit /B 2
)

cd %BCGL_ROOT%

if "%1" == "run" (
    echo Run %2 %cd%
    %2
)

exit /B 0
