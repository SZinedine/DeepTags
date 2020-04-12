#!/bin/bash

_APP_NAME=DeepTags
_APP_BIN_NAME=deeptags
_PKG_DIR=$(readlink -f $(dirname $0))
_ROOT=$(readlink -f $_PKG_DIR/..)
_APP_BIN=$_ROOT/$_APP_BIN_NAME
_RC_DIR=$_PKG_DIR/resources
_DESKTOP_FILE=$_RC_DIR/$_APP_BIN_NAME.desktop
_APPDATA_FILE=$_RC_DIR/$_APP_BIN_NAME.appdata.xml
_ICON_FILE=$_ROOT/$_APP_BIN_NAME.png
_ICON_DIR=$_RC_DIR/icons
_TRANSLATIONS=$_ROOT/locale/*.qm
_APP_VERSION=$($_APP_BIN --version)
_APPIMAGE_DIR=$_PKG_DIR/deeptags_"$_APP_VERSION"
_OUPUT="$_APPIMAGE_DIR"-86_x64.AppImage
_LINUXDEPLOYQT="$_PKG_DIR"/linuxdeployqt-*-x86_64.AppImage
export VERSION=$_APP_VERSION    # used by linuxdeployqt

rm -rf $_APPIMAGE_DIR

# Check if we are on Linux
if [ "$(uname)" != "Linux" ]; then
    >&2 echo "This script only works on linux."
    exit 1
fi

if [ ! -f "$_APP_BIN" ]; then
    >&2 echo "the binary of $_APP_NAME doesn't exist."
    exit 1
fi

# download linuxdeployqt if it doesn't exist
if [ ! -f $_LINUXDEPLOYQT ]; then
    >&2 echo "LinuxDeployQt doesn't exit."
    echo "Downloading LinuxDeployQt..."
    wget -P $_PKG_DIR -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
    chmod +x $_LINUXDEPLOYQT
fi


echo "creating the directory structure"
mkdir -p $_APPIMAGE_DIR/usr/bin
mkdir -p $_APPIMAGE_DIR/usr/translations
mkdir -p $_APPIMAGE_DIR/usr/share/applications
mkdir -p $_APPIMAGE_DIR/usr/share/metainfo

echo "copying necessary files"
cp $_DESKTOP_FILE   $_APPIMAGE_DIR/
cp $_DESKTOP_FILE   $_APPIMAGE_DIR/usr/share/applications
cp -r $_ICON_DIR    $_APPIMAGE_DIR/usr/share/
cp $_ICON_FILE      $_APPIMAGE_DIR/
cp $_TRANSLATIONS   $_APPIMAGE_DIR/usr/translations/
cp $_APP_BIN        $_APPIMAGE_DIR/usr/bin
cp $_APPDATA_FILE   $_APPIMAGE_DIR/usr/share/metainfo/


# if --add-libstdc++ is provided, bundle libstdc++.so.x 
if [ "$*" == "--add-libstdc++" ]; then
    echo "copying libstdc++"
    mkdir -p $_APPIMAGE_DIR/usr/lib
    cp /usr/lib/x86_64-linux-gnu/libstdc++.so* $_APPIMAGE_DIR/usr/lib/
fi


echo "creating the appimage"
RESULT=$($_LINUXDEPLOYQT $_APPIMAGE_DIR/usr/bin/$_APP_BIN_NAME -appimage -bundle-non-qt-libs)

if [ $? != 0 ]; then
    >&2 echo "ERROR. LinuxDeployQt failed."
fi


echo "deleting temporary files"
rm -rf $_APPIMAGE_DIR
