#!/usr/bin/env bash

if [ "$1" = "clean" ]; then
    echo Cleaning...
    rm -rf build
    exit 0
fi

if [ "$1" = "install" ]; then
    echo Installing...
    sudo dpkg --install build/*.deb
    exit 0
fi

mkdir -p build
pushd build
cmake ..
cmake --build .
cpack
popd

