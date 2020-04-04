#!/bin/bash

_APP_VERSION=0.5
_APP_NAME=DeepTags
_PKG_DIR=$(realpath $(dirname $0))
_ROOT=$(realpath $_PKG_DIR/..)
_DESKTOP_FILE=$_ROOT/$_APP_NAME.desktop
_ICON_FILE=$_ROOT/$_APP_NAME.png
_TRANSLATIONS=$_ROOT/locale/*.qm
_APP_BIN=$_ROOT/build/release/$_APP_NAME
_APPIMAGE_DIR=$_PKG_DIR/deeptags_"$_APP_VERSION"
_OUPUT="$_APPIMAGE_DIR"-86_x64.AppImage
_LINUXDEPLOYQT="$_PKG_DIR"/linuxdeployqt-*-x86_64.AppImage

rm -rf $_APPIMAGE_DIR

# Check if we are on Linux
if [ "$(uname)" != "Linux" ]; then
    echo "This script only works on linux."
    exit 1
fi

if [ ! -f "$_APP_BIN" ]; then
    echo "the binary of $_APP_NAME doesn't exist."
    exit 1
fi

mkdir -p $_APPIMAGE_DIR/usr/bin
mkdir -p $_APPIMAGE_DIR/usr/translations
mkdir -p $_APPIMAGE_DIR/usr/share/applications
mkdir -p $_APPIMAGE_DIR/usr/share/icons/hicolor/256x256/apps/

echo "copying necessary files"
cp $_DESKTOP_FILE $_APPIMAGE_DIR/usr/share/applications
cp $_ICON_FILE $_APPIMAGE_DIR/usr/share/icons/hicolor/256x256/apps/
cp $_DESKTOP_FILE $_APPIMAGE_DIR/
cp $_ICON_FILE $_APPIMAGE_DIR/
cp $_TRANSLATIONS $_APPIMAGE_DIR/usr/translations/
cp $_APP_BIN $_APPIMAGE_DIR/usr/bin

echo "creaeting the appimage"
$_LINUXDEPLOYQT $_APPIMAGE_DIR/usr/bin/$_APP_NAME -appimage -bundle-non-qt-libs


echo "deleting temporary files"
rm -rf $_APPIMAGE_DIR
