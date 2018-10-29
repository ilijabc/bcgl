(
    cd app/android &&
    adb shell am force-stop info.djukic.angrypixelbots2 &&
    ./gradlew installDebug &&
    adb shell am start info.djukic.angrypixelbots2/.MainActivity
)
