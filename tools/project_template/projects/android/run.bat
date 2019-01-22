@echo off

adb shell am force-stop {{bcapp:company}}.{{bcapp:name}}

pushd %~dp0

call gradle installDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

adb shell am start {{bcapp:company}}.{{bcapp:name}}/info.djukic.bcgl.BCGLActivity

popd
exit /B 0
