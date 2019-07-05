@echo off

adb shell am force-stop com.example.test_app

pushd %~dp0

call gradle installDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

adb shell am start com.example.test_app/info.djukic.bcgl.BCGLActivity

popd
exit /B 0
