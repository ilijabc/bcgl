#!/bin/bash

(
	adb shell am force-stop com.bcgl.demo &&
	cd $(dirname $0) &&
	gradle installDebug &&
	adb shell am start com.bcgl.demo/info.djukic.bcgl.BCGLActivity
)
