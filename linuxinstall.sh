BIN=tmp_bin/DeepTags
PROG=DeepTags
ICO=DeepTags.png
DSKTP=DeepTags.desktop

BIN_DESTINATION=/usr/bin/
ICO_DESTINATION1=/usr/share/icons/hicolor/256x256/apps/
ICO_DESTINATION2=/usr/share/icons/hicolor/
DSKTP_DESTINATION=/usr/share/applications/


# Check if we are on Linux
if [ "$(uname)" != "Linux" ]; then
    echo "This script only works on linux."
    exit 1
fi


# asking
if [ -f "$BIN" ]; then
    echo "this will install $PROG in your system."
    read -p "Do you want to proceed? (y/n) " yesOrNot
else
    echo "ERROR. Please compile DeepTags before installing it."
    exit 1
fi


# installing
if [ "$yesOrNot" == "y" ] ;
then
    sudo cp $BIN $BIN_DESTINATION
    sudo cp $ICO $ICO_DESTINATION1
    sudo cp $ICO $ICO_DESTINATION2
    sudo cp $DSKTP $DSKTP_DESTINATION

    echo "Installation done."
else 
    exit 0
fi

# exit 0
##############################################

# creating a simlink to the desktop
read -p "Do you want to create a shortcut to your desktop? (y/n) " yesOrNot 

if [ "$yesOrNot" == "y" ] ;
then
    cp $DSKTP /home/$USER/Desktop/
fi

