#!/bin/bash

(
	adb shell am force-stop {{bcapp:company}}.{{bcapp:name}} &&
	cd $(dirname $0) &&
	gradle installDebug &&
	adb shell am start {{bcapp:company}}.{{bcapp:name}}/info.djukic.bcgl.BCGLActivity
)
