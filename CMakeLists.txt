cmake_minimum_required(VERSION 3.16)

project(deeptags LANGUAGES CXX VERSION 0.8.0)
add_definitions(-DDEEPTAGS_VERSION="${CMAKE_PROJECT_VERSION}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(CMAKE_UNITY_BUILD TRUE)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)

set(PROJECT_SOURCES
# source files
    src/MainWindow.cpp
    src/MainWindow.h
    src/DocumentContentView.cpp
    src/DocumentContentView.h
    src/Document.cpp
    src/Document.h
    src/DocumentUtils.cpp
    src/DocumentUtils.h
    src/TagsTreeView.cpp
    src/TagsTreeView.h
    src/TagTreeItem.cpp
    src/TagTreeItem.h
    src/TagTreeModel.cpp
    src/TagTreeModel.h
    src/TagTreeDelegate.cpp
    src/TagTreeDelegate.h
    src/DocumentsListView.cpp
    src/DocumentsListView.h
    src/DocumentListItem.cpp
    src/DocumentListItem.h
    src/DocumentListModel.cpp
    src/DocumentListModel.h
    src/DocumentListDelegate.cpp
    src/DocumentListDelegate.h
    src/Settings.cpp
    src/Settings.h
    src/SettingsDialog.cpp
    src/SettingsDialog.h
    src/DocumentInfoDialog.cpp
    src/DocumentInfoDialog.h
    src/DataDirectoryCentralWidget.cpp
    src/DataDirectoryCentralWidget.h
    src/DataDirectoryDialog.cpp
    src/DataDirectoryDialog.h
    src/ExternalReadersCentralWidget.cpp
    src/ExternalReadersCentralWidget.h
    src/ExternalReadersDialog.cpp
    src/ExternalReadersDialog.h

# UI files
    src/MainWindow.ui
    src/DocumentInfoDialog.ui
    src/DataDirectoryCentralWidget.ui
    src/DataDirectoryDialog.ui
    src/ExternalReadersCentralWidget.ui
    src/ExternalReadersDialog.ui
    src/SettingsDialog.ui

# Resource files
    DeepTags.qrc
    src/3rdParty/QBreeze/qbreeze.qrc
)

option(TEST "TEST" OFF)
if (TEST)
    message("Building tests")
    list(APPEND PROJECT_SOURCES tests/DocumentUtilsTest.cpp)
else() 
    list(APPEND PROJECT_SOURCES src/main.cpp)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(QAPPLICATION_CLASS QApplication)
add_subdirectory(src/3rdParty/SingleApplication)
# add_subdirectory(src/3rdParty/qmarkdowntextedit)

list(APPEND PROJECT_SOURCES
    src/3rdParty/qmarkdowntextedit/markdownhighlighter.cpp
    src/3rdParty/qmarkdowntextedit/markdownhighlighter.h
    src/3rdParty/qmarkdowntextedit/qownlanguagedata.cpp
    src/3rdParty/qmarkdowntextedit/qownlanguagedata.h
)

add_executable(deeptags ${PROJECT_SOURCES})

target_link_libraries(deeptags
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Concurrent
    PRIVATE SingleApplication::SingleApplication
)

target_compile_options(deeptags PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
         -Wall -Wextra>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)


if (TEST)
    target_link_libraries(deeptags PRIVATE Qt5::Test)
endif()


# cmake -DBENCHMARK=ON
option(BENCHMARKS "BENCHMARKS" OFF)
if (BENCHMARK)
    message("Building with benchmarks")
    add_definitions(-DBENCHMARKS)
endif()


set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.deeptags)

set_target_properties(deeptags PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS deeptags
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
