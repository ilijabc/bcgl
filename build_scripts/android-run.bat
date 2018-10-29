rem @echo off

rem %1 - gradle app path
rem %2 - package name

set ROOT="%cd%"

cd %1

adb shell am force-stop %2

call gradlew.bat installDebug

adb shell am start %2/.MainActivity

cd %ROOT%

exit /B 0
