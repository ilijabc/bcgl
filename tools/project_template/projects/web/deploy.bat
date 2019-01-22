@echo off

rem pass destination path as %1

call %~dp0\build.bat

copy %~dp0\build %1 /Y
