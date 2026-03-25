@echo off

pushd %~dp0

rmdir /S /Q build

popd
exit /B 0
