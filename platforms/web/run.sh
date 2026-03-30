#!/bin/bash

(
    $(dirname $0)/build.sh &&
    cd $(dirname $0) &&
    echo "Open http://localhost:8000/ in web browser" &&
    python -m http.server 8000
)
