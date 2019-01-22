@echo off

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\win32"
)

rmdir /S /Q %BCGL_OUTPUT%
