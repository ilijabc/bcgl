#!/bin/bash
TARGET=${1:-desktop}
COMMAND=${2:-build}
platforms/$TARGET/$COMMAND.sh
