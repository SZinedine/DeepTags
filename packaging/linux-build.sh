#!/usr/bin/bash

SCRIPT_DIR=$(readlink -f $(dirname $0))
PROJECT_DIR=$(readlink -f $SCRIPT_DIR/..)
BUILD_DIR="$PROJECT_DIR/build"
BIN_NAME="deeptags"
BIN_PATH="$BUILD_DIR/$BIN_NAME"

cd "$PROJECT_DIR"

echo "Creating the build directory"
mkdir -p "$BUILD_DIR"
rm -f "$BIN_PATH"

echo "running cmake"
cmake -B "$BUILD_DIR" -DBENCHMARK=OFF -DTEST=OFF -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"

echo "building"
make -j$(nproc) -C build


if [ ! -f "$BIN_PATH" ]; then
    echo "the binary of $_APP_NAME doesn't exist."
    exit 1
else
    echo "successfully built DeepTags"
    exit 0
fi
