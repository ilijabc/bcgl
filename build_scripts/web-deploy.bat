@echo off

rem pass executable path as %1

call %~dp0\web-build.bat

if "%BCGL_OUTPUT%" == "" (
    set BCGL_OUTPUT="_output\web"
)

rem xcopy %BCGL_OUTPUT%\%1 %HTDOCS% /E /Y
copy %BCGL_OUTPUT%\%1 %HTDOCS% /Y
