@echo off

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\web"
)

rmdir /S /Q %BCGL_OUTPUT%
