@echo off

call %~dp0\build.bat
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

pushd %~dp0\..\..
%~dp0\build\__bcgl_project_name__.exe %*
popd
