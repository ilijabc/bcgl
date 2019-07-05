#!/bin/bash

(
	adb shell am force-stop com.example.test_app &&
	cd $(dirname $0) &&
	gradle installDebug &&
	adb shell am start com.example.test_app/info.djukic.bcgl.BCGLActivity
)
