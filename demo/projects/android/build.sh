#!/bin/bash

(
	cd $(dirname $0) &&
	gradle assembleDebug
)
