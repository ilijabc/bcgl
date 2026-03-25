#!/bin/bash

(
	$(dirname $0)/build.sh &&
	cd $(dirname $0)/../.. &&
	$(dirname $0)/build/__bcgl_project_name__ $@
)
