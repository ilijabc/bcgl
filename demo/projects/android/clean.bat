@echo off

pushd %~dp0

rmdir /S /Q .gradle
rmdir /S /Q .externalNativeBuild
rmdir /S /Q build

popd
exit /B 0
