#!/bin/bash

(
	$(dirname $0)/build.sh &&
	cd $(dirname $0)/../.. &&
	$(dirname $0)/build/test_app
)
