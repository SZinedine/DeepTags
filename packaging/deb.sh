#!/bin/bash

_APP_VERSION=0.5
_APP_NAME=DeepTags
_PKG_DIR=$(realpath $(dirname $0))
_ROOT=$(realpath $_PKG_DIR/..)
_DESKTOP_FILE=$_ROOT/$_APP_NAME.desktop
_ICON_FILE=$_ROOT/$_APP_NAME.png
_APP_BIN=$_ROOT/build/release/$_APP_NAME
_DEB_DIR=$_PKG_DIR/deeptags_"$_APP_VERSION"_amd64


# Check if we are on Linux
if [ "$(uname)" != "Linux" ]; then
    echo "This script only works on linux."
    exit 1
fi

if [ ! -f "$_APP_BIN" ]; then
    echo "the binary of $_APP_NAME doesn't exist."
    exit 1
fi

echo "creating directories for packaging"
mkdir -p $_DEB_DIR/DEBIAN
mkdir -p $_DEB_DIR/usr/bin
mkdir -p $_DEB_DIR/usr/share/applications
mkdir -p $_DEB_DIR/usr/share/icons/hicolor/256x256/apps/

echo "creating control file"
cat >> $_DEB_DIR/DEBIAN/control <<EOL
Package: $_APP_NAME
Version: $_APP_VERSION
Architecture: amd64
Essential: no
Priority: optional
Section: utils
Depends: libqt5widgets5 (>= 5.6), libqt5gui5 (>= 5.6), libqt5network5 (>= 5.6), libqt5core5a (>= 5.6), libc6, libgcc1, libstdc++6, libx11-6
Maintainer: Zineddine SAIBI <saibi.zineddine@yahoo.com>
Homepage: https://www.github.com/SZinedine/DeepTags
Description: A Markdown notes manager
EOL

echo "copying necessary files"
cp $_DESKTOP_FILE $_DEB_DIR/usr/share/applications
cp $_ICON_FILE $_DEB_DIR/usr/share/icons/hicolor/256x256/apps/
cp $_APP_BIN $_DEB_DIR/usr/bin


echo "creating the deb package"
dpkg-deb --build $_DEB_DIR

echo "deleting temporary files"
rm -rf _DEB_DIR
