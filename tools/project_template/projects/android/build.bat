@echo off

pushd %~dp0

call gradle assembleDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

popd
exit /B 0
