@echo off

call %~dp0\build.bat

pushd %~dp0\..\..
%~dp0\build\{{bcapp:name}}.exe
popd
