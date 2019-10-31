#!/bin/bash

(
    cd $(dirname $0) &&
    rm -rf .gradle
    rm -rf .externalNativeBuild
    rm -rf build
)
