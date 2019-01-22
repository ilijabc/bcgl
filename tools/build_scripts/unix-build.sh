mkdir -p _output/linux

(
    cd _output/linux &&
    cmake -G 'Unix Makefiles' ../.. &&
    make
)
