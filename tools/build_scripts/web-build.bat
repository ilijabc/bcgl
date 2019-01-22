@echo off

set BCGL_ROOT="%cd%"

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\web"
)

echo BCGL_OUTPUT=%BCGL_OUTPUT%
echo BCGL_ROOT=%BCGL_ROOT%

cd /D %EMSDK%
call emsdk_env.bat

cd /D %BCGL_ROOT%
mkdir %BCGL_OUTPUT%
cd %BCGL_OUTPUT%

call emcmake cmake %BCGL_ROOT%
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

mingw32-make
if not %ERRORLEVEL% == 0 (
    exit /B 2
)

cd %BCGL_ROOT%

exit /B 0
