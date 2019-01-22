@echo off

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\android"
)

rmdir /S /Q %BCGL_OUTPUT%
