#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>

#include <QSplitter>
#include <QVariant>
#include <QStringList>
#include <QMenu>
#include "mainwindow.h"

struct Settings : public QObject {

    static void openFileAction(QAction* action);
    static void openFile(QString editor, const fs::path& path, QWidget* parent=nullptr);
    /**
     * save the main window related settings
     */
    static void saveUiSettings(const QSize& windowSize, QByteArray splitterState);
    /**
     * load the saved settings directely into the splitter
     */
    static void loadSplitterState(QSplitter* splitter);
    static void saveSplitterState(QSplitter* splitter);
    static void loadWindowSize(MainWindow* w);
    static bool setDataDirectory();
    static QString dataDirectory();
    static bool dataDirectoryIsSet();
    /**
     * Expand TagItems or not
     */
    static void expand(const bool& expanded);
    static bool expandedItems();
    /**
     * the stored markdown editor
     */
    static QStringList mdEditors();
    static void saveEditors(const QStringList& lst);
    static void saveMainEditor(const QString& editor);
    static QString mainMdEditor();
    /**
     * save the path everytime a document is opened
     */
    static void saveRecentlyOpenedFile(const fs::path& p);
    /**
     * write the file paths into the filesystem (save them)
     */
    static void saveRecentlyOpenedFiles(QStringList& paths);
    /**
     * get the saved paths in a list of strings
     */
    static QStringList getRawRecentlyOpenedFiles();
    /**
     * construct a QAction from each stored path and add them
     * into a provided menu after it is cleared
     */
    static QMenu*  getActionsRecentlyOpenedFiles(QMenu * menu);
    static void eraseRecentlyOpenedFiles();
    /**
     * Save the theme chosen by the user
     */
    static void saveTheme(QAction* ac);
    static void loadTheme(QActionGroup* ag);
    static void applyTheme(const QString& theme);
};

#endif

