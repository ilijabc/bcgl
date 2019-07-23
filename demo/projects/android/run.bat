@echo off

adb shell am force-stop com.bcgl.demo

pushd %~dp0

call gradle installDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

adb shell am start com.bcgl.demo/info.djukic.bcgl.BCGLActivity

popd
exit /B 0
