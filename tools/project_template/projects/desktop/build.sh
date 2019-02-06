#!/bin/bash

(
	mkdir -p $(dirname $0)/build &&
    cd $(dirname $0)/build &&
    cmake -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug .. &&
    make
)
