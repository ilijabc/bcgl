@echo off

adb shell am force-stop __bcgl_project_company__.__bcgl_project_name__

pushd %~dp0

call gradle installDebug
if not %ERRORLEVEL% == 0 (
    popd
    exit /B 1
)

adb shell am start __bcgl_project_company__.__bcgl_project_name__/info.djukic.bcgl.BCGLActivity

popd
exit /B 0
