#!/bin/bash

(
	mkdir -p $(dirname $0)/build &&
    cd $(dirname $0)/build &&
    emcmake cmake .. &&
    make
)
