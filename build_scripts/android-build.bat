@echo off

rem pass gradle path as %1

set ROOT="%cd%"

cd %1

call gradlew.bat assembleDebug
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

cd %ROOT%

exit /B 0
