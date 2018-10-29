export ANDROID_API_LEVEL=26
export ANDROID_ARCH_ABI=armeabi-v7a
export BUILD_TYPE=debug

mkdir -p _output/android

(
    cd _output/android &&
    cmake -G "Unix Makefiles" \
        -DCMAKE_SYSTEM_NAME=Android \
        -DANDROID_PLATFORM=android-${ANDROID_API_LEVEL} \
        -DANDROID_ABI=${ANDROID_ARCH_ABI} \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DCMAKE_SYSTEM_VERSION=${ANDROID_API_LEVEL} \
        -DCMAKE_ANDROID_ARCH_ABI=${ANDROID_ARCH_ABI} \
        -DANDROID_STL=c++_shared \
        -DBUILD_TYPE=${BUILD_TYPE} \
        -DBUILD_PLATFORM=Android \
        -DBUILD_TARGET=Arm \
        -DTOOLCHAIN_ROOT=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64 \
        ../..
    make
)
