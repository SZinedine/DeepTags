#!/usr/bin/bash

SCRIPT_DIR=$(readlink -f $(dirname $0))
PROJECT_DIR=$(readlink -f $SCRIPT_DIR/..)
BUILD_DIR="$PROJECT_DIR/build"
BIN_NAME="deeptags"
BIN_PATH="$BUILD_DIR/$BIN_NAME"

PKG_DIR="$PROJECT_DIR/packaging"
RC_DIR="$PKG_DIR/resources"
DESKTOP_FILE="$RC_DIR/$BIN_NAME.desktop"
APPDATA_FILE="$RC_DIR/$BIN_NAME.appdata.xml"
ICONS_DIR="$RC_DIR/icons"


# Check if we are on Linux
if [ "$(uname)" != "Linux" ]; then
    echo "This script only works on linux."
    exit 1
fi

if [ ! -f "$BIN_PATH" ]; then
    echo "the binary of $BIN_PATH doesn't exist. You have to build the project first"
    exit 1
fi



VERSION=$($BIN_PATH --version | awk '{print $2}')
DEB_DIR=$PKG_DIR/deeptags_"$VERSION"_amd64
rm -rf $DEB_DIR


mkdir -p $DEB_DIR/DEBIAN
mkdir -p $DEB_DIR/usr/bin
mkdir -p $DEB_DIR/usr/share/applications
mkdir -p $DEB_DIR/usr/share/metainfo

echo "copying files"
cp    $DESKTOP_FILE  $DEB_DIR/usr/share/applications
cp -r $ICONS_DIR     $DEB_DIR/usr/share/
cp    $BIN_PATH      $DEB_DIR/usr/bin
cp    $APPDATA_FILE  $DEB_DIR/usr/share/metainfo/


echo "creating the control file"
cat >> $DEB_DIR/DEBIAN/control <<EOL
Package: $BIN_NAME
Version: $VERSION
Architecture: amd64
Essential: no
Priority: optional
Section: utils
Depends: libqt5widgets5 (>= 5.15), libqt5gui5 (>= 5.15), libqt5network5 (>= 5.15), libqt5core5a (>= 5.15)
Maintainer: Zineddine SAIBI <saibi.zineddine@yahoo.com>
Homepage: https://www.github.com/SZinedine/DeepTags
Description: A markdown notes manager
EOL


echo "creating the deb package"
dpkg-deb --build $DEB_DIR

if [ -n "$DIST" ]; then
    echo "adding the distribution name into the output filename"
    mv $DEB_DIR.deb $DEB_DIR-$DIST.deb
fi

echo "deleting temporary files"
rm -rf $DEB_DIR

