#-------------------------------------------------
#
# Project created by QtCreator 2019-07-14T23:10:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeepTags
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000
CONFIG += c++17 -lstdc++fs -pthread
QMAKE_CXXFLAGS += -std=c++17 -lstdc++fs
TRANSLATIONS = locale/deeptags_fr.ts
SUBDIRS = src
PARENT_DIR = $${PWD}/
SRC_DIR = $${PARENT_DIR}/src
GUI_DIR = $${SRC_DIR}/gui
ELEM_DIR = $${SRC_DIR}/element/
BUILD_DIR = $${PARENT_DIR}/build

ELEM_HEADERS =  $${ELEM_DIR}/element.h $${ELEM_DIR}/baseelement.h
ELEM_SOURCES =  $${ELEM_DIR}/element.cpp $${ELEM_DIR}/baseelement.cpp
GUI_HEADERS  =  $${GUI_DIR}/fileitem.h $${GUI_DIR}/filescontainer.h \
                $${GUI_DIR}/mainwindow.h $${GUI_DIR}/settings.h \
                $${GUI_DIR}/tagitem.h $${GUI_DIR}/tagscontainer.h \
                $${GUI_DIR}/elementdialog.h
GUI_SOURCES  =  $${GUI_DIR}/fileitem.cpp $${GUI_DIR}/filescontainer.cpp \
                $${GUI_DIR}/mainwindow.cpp $${GUI_DIR}/settings.cpp \
                $${GUI_DIR}/tagitem.cpp $${GUI_DIR}/tagscontainer.cpp \
                $${GUI_DIR}/elementdialog.cpp
SOURCES += $${SRC_DIR}/main.cpp $${ELEM_SOURCES} $${GUI_SOURCES}
HEADERS += $${ELEM_HEADERS} $${GUI_HEADERS}

# put output files in different directories depending on release || debug build
CONFIG(debug, debug|release) {
    DESTDIR = $${BUILD_DIR}/debug
}
CONFIG(release, debug|release) {
    DESTDIR = $${BUILD_DIR}/release
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rc


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += DeepTags.qrc
