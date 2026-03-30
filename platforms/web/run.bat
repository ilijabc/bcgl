@echo off

call %~dp0\build.bat
if not %ERRORLEVEL% == 0 (
    exit /B 1
)

pushd %~dp0
echo "Open http://localhost:8000/ in web browser"
python -m http.server 8000
popd
