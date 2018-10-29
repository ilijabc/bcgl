@echo off

set ROOT="%cd%"

cd %1

call gradlew.bat clean
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

cd %ROOT%

exit /B 0
