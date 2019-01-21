(
    cd app/android &&
    adb shell am force-stop info.djukic.bcgl/.BCGLActivity &&
    ./gradlew installDebug &&
    adb shell am start info.djukic.bcgl/.BCGLActivity
)
