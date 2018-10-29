./build_scripts/unix-build.sh

if [ -z $BCGL_OUTPUT ]; then
    export BCGL_OUTPUT="_output/linux"
fi

$BCGL_OUTPUT/app/desktop/ap2
