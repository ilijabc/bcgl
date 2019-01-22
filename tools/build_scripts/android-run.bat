@echo off

rem pass gradle path as %1
rem pass app package name as %2

set ROOT="%cd%"

cd %1

adb shell am force-stop %2

call gradlew.bat installDebug
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

adb shell am start %2/%3

cd %ROOT%

exit /B 0
