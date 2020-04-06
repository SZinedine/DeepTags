#!/bin/bash

_APP_NAME=DeepTags
_PKG_DIR=$(readlink -f $(dirname $0))
_ROOT=$(readlink -f $_PKG_DIR/..)
_APP_BIN=$_ROOT/build/release/$_APP_NAME
_RC_DIR=$_PKG_DIR/resources
_DESKTOP_FILE=$_RC_DIR/$_APP_NAME.desktop
_ICON_FILE=$_ROOT/$_APP_NAME.png
_ICON_DIR=$_RC_DIR/icons
_TRANSLATIONS=$_ROOT/locale/*.qm
_APP_VERSION=$($_APP_BIN --version)
_DEB_DIR=$_PKG_DIR/deeptags_"$_APP_VERSION"_amd64


rm -rf $_DEB_DIR


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


echo "copying necessary files"
cp $_DESKTOP_FILE   $_DEB_DIR/usr/share/applications
cp -r $_ICON_DIR    $_DEB_DIR/usr/share/
cp $_APP_BIN        $_DEB_DIR/usr/bin


echo "creating control file"
cat >> $_DEB_DIR/DEBIAN/control <<EOL
Package: $_APP_NAME
Version: $_APP_VERSION
Architecture: amd64
Essential: no
Priority: optional
Section: utils
Depends: libqt5widgets5 (>= 5.6), libqt5gui5 (>= 5.6), libqt5network5 (>= 5.6), libqt5core5a (>= 5.6), libc6, libgcc1, libstdc++6, libx11-6, libgl1-mesa-dev
Maintainer: Zineddine SAIBI <saibi.zineddine@yahoo.com>
Homepage: https://www.github.com/SZinedine/DeepTags
Description: A Markdown notes manager
EOL


echo "creating the deb package"
dpkg-deb --build $_DEB_DIR


echo "deleting temporary files"
rm -rf $_DEB_DIR
