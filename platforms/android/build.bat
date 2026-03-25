@echo off

pushd %~dp0

call gradlew assembleDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

popd
exit /B 0
