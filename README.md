# BCGL

This lib (let's call it that way) was privately used for in-house projects.
I decided to publish it so maybe it is not convenient to use since nothing
is documented. This is a work in progress...

## Init a new project

Do following to initialize a new project based on BCGL:

```
git clone https://github.com/ilijabc/bcgl.git
./bcgl/tools/create_project.py -C my.company -N myapp -T "My App"
```

## Dependencies

Libraries:
- STB - https://github.com/nothings/stb
- GLFW - https://github.com/glfw/glfw
- GLAD - https://github.com/Dav1dde/glad
- PAR - https://github.com/prideout/par

Build tools:
- CMake - https://cmake.org/
- Mingw-w64 - https://mingw-w64.org/doku.php
- Emscripten - https://kripken.github.io/emscripten-site/
- Android SDK/NDK - https://developer.android.com/
- Python - https://www.python.org/

Supported IDEs:
- Sublime Text
- Visual Studio Code

## Build on Windows

### Desktop

Prepare the environment:
- Get CMake
- Get Mingw-w64
- Get GLFW

Build and run:
```
projects\desktop\build.bat
projects\desktop\run.bat
```

### Android

Prepare the environment:
- Get Android SDK
- Get Android NDK
- Get gradle

Build and run:
```
projects\android\build.bat
projects\android\run.bat
```

### Web

Prepare the environment:
- Get emscripten

Build:
```
projects\web\build.bat
```

## Build on Linux

### Desktop

Prepare the environment:
```
sudo apt install g++
sudo apt install libglfw3-dev
```

Build and run:
```
./projects/desktop/build.sh
./projects/desktop/run.sh
```

### Android

Prepare the environment:
```
# put everything in android folder
mkdir ~/android
cd ~/android
# get sdk and tools
wget https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip
wget https://downloads.gradle.org/distributions/gradle-5.2.1-bin.zip
wget https://github.com/ninja-build/ninja/releases/download/v1.9.0/ninja-linux.zip
# unpack all
unzip sdk-tools-linux-4333796.zip
unzip gradle-5.2.1-bin.zip
unzip ninja-linux.zip
# setup
./tools/bin/sdkmanager "build-tools;28.0.3" "platforms;android-28" "ndk-bundle"
chmod +x ninja
export ANDROID_HOME=~/android
export ANDROID_NDK=~/android/ndk-bundle
export PATH=~/android/gradle-5.2.1/bin:~/android:$PATH
```

Build and run:
```
./projects/android/build.sh
./projects/android/run.sh
```

### Web

Prepare the environment:
```
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

Build:
```
./projects/web/build.sh
```

## Credits

This project is inspired by following projects:
- raylib - https://github.com/raysan5/raylib
- mathc - https://github.com/felselva/mathc
- libGDX - https://github.com/libgdx/libgdx
