#!/bin/bash

(
	adb shell am force-stop __bcgl_project_company__.__bcgl_project_name__ &&
	cd $(dirname $0) &&
	gradle installDebug &&
	adb shell am start __bcgl_project_company__.__bcgl_project_name__/info.djukic.bcgl.BCGLActivity
)
