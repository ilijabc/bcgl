@echo off

pushd %EMSDK%
call emsdk_env.bat
popd

mkdir %~dp0\build
pushd %~dp0\build

call emcmake cmake ..
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
