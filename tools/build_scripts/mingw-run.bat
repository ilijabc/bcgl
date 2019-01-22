@echo off

rem pass executable path as %1

call %~dp0\mingw-build.bat

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\win32"
)

%BCGL_OUTPUT%\%1 %2
