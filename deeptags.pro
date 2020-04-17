#-------------------------------------------------
#
# Project created by QtCreator 2019-07-14T23:10:28
#
#-------------------------------------------------

DEFINES += DEEPTAGS_VERSION=\\\"0.5\\\"
QT += core gui widgets
TARGET = deeptags
TEMPLATE = app
CONFIG += -pthread
RESOURCES += deeptags.qrc
TRANSLATIONS = locale/deeptags_fr.ts
RC_ICONS = ./images/deeptags.ico
SUBDIRS = src
PARENT_DIR = $$PWD/
DESTDIR = $$PARENT_DIR
SRC_DIR = $$PARENT_DIR/src
GUI_DIR = $$SRC_DIR/gui
ELEM_DIR = $$SRC_DIR/element/
BUILD_DIR = $$PARENT_DIR/build
ELEM_HEADERS =  $$ELEM_DIR/element.h $$ELEM_DIR/baseelement.h
ELEM_SOURCES =  $$ELEM_DIR/element.cpp $$ELEM_DIR/baseelement.cpp
GUI_HEADERS  =  $$GUI_DIR/fileitem.h $$GUI_DIR/filescontainer.h \
                $$GUI_DIR/mainwindow.h $$GUI_DIR/settings.h \
                $$GUI_DIR/tagitem.h $$GUI_DIR/tagscontainer.h \
                $$GUI_DIR/elementdialog.h $$GUI_DIR/readersdialog.h
GUI_SOURCES  =  $$GUI_DIR/fileitem.cpp $$GUI_DIR/filescontainer.cpp \
                $$GUI_DIR/mainwindow.cpp $$GUI_DIR/settings.cpp \
                $$GUI_DIR/tagitem.cpp $$GUI_DIR/tagscontainer.cpp \
                $$GUI_DIR/elementdialog.cpp $$GUI_DIR/readersdialog.cpp
SOURCES += $$SRC_DIR/main.cpp $$ELEM_SOURCES $$GUI_SOURCES
HEADERS += $$ELEM_HEADERS $$GUI_HEADERS


# Prevent qmake from appending the gnu++ standard that overrides this configuration
STD = 14
msvc:QMAKE_CXXFLAGS += /std:c++$$STD
gcc:QMAKE_CXXFLAGS += -std=c++$$STD
gcc:QMAKE_CXXFLAGS_CXX11    = -std=c++$$STD
gcc:QMAKE_CXXFLAGS_CXX14    = -std=c++$$STD
gcc:QMAKE_CXXFLAGS_CXX1Z    = -std=c++17
gcc:QMAKE_CXXFLAGS_GNUCXX11 = -std=c++$$STD
gcc:QMAKE_CXXFLAGS_GNUCXX14 = -std=c++$$STD
gcc:QMAKE_CXXFLAGS_GNUCXX1Z = -std=c++17
message("DeepTags will be compiled with $$QMAKE_CXX and c++$$STD")

# put output files in different directories depending on release || debug build
CONFIG(debug, debug|release) {
    OUTFILES = $$BUILD_DIR/debug
}
CONFIG(release, debug|release) {
    OUTFILES = $$BUILD_DIR/release
}

OBJECTS_DIR = $$OUTFILES/obj
MOC_DIR = $$OUTFILES/moc
RCC_DIR = $$OUTFILES/rc

# make QBreeze an optional dependency
exists(3rdParty/QBreeze/qbreeze.qrc){
    RESOURCES += 3rdParty/QBreeze/qbreeze.qrc
    DEFINES += INCLUDE_QBREEZE
} else {
    message("QBreeze not included.")
}

# if QMarkdownTextEdit is cloned
exists(3rdParty/qmarkdowntextedit/qmarkdowntextedit.pri) {
    include (3rdParty/qmarkdowntextedit/qmarkdowntextedit.pri)
    SOURCES += $$GUI_DIR/editorwidget.cpp
    HEADERS += $$GUI_DIR/editorwidget.h
    DEFINES += INSIDE_EDITOR
} else {
    message("QMarkdownTextEdit not included.")
}

# if SingleApplication is cloned
exists(3rdParty/SingleApplication/singleapplication.pri) {
    include (3rdParty/SingleApplication/singleapplication.pri)
    DEFINES += INCLUDE_SINGLE_APPLICATION
    DEFINES += QAPPLICATION_CLASS=QApplication
}else {
    message("SingleApplication included.")
}


# Default rules for deployment.
qnx: target.path = /tmp/$$TARGET/bin
else: unix:!android: target.path = /opt/$$TARGET/bin
!isEmpty(target.path): INSTALLS += target

