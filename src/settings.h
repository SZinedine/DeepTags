/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2020  Zineddine Saibi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QHash>

class QSplitter;
class QMenu;
class QVariant;
class QString;
class QStringList;
class QWidget;
class QActionGroup;
class MainWindow;
class QSize;
class QByteArray;
class QAction;

namespace Settings {
    void saveString(const QString& group, const QString& label, const QString& value);
    QString getString(const QString& group, const QString& label);
    void saveStringList(const QString& group, const QString& label, const QStringList& value);
    QStringList getStringList(const QString& group, const QString& label);

    void openFileAction(QAction* action);
    void openFile(QString editor, const QString& path, QWidget* parent = nullptr);
    /**
     * save the main window related settings
     */
    void saveUiSettings(const QSize& windowSize, const QByteArray& splitterState);
    /**
     * load the saved settings directely into the splitter
     */
    void loadSplitterState(QSplitter* splitter);
    void saveSplitterState(QSplitter* splitter);
    void loadWindowSize(MainWindow* w);
    bool setDataDirectory(QString dataDirectory);
    QString dataDirectory();
    bool dataDirectoryIsSet();
    /**
     * Expand TagItems or not
     */
    void expand(const bool& expanded);
    bool expandedItems();
    /**
     * the stored markdown editor
     */
    QStringList mdEditors();
    void saveEditors(const QStringList& lst);
    void saveMainEditor(const QString& editor);
    QString mainMdEditor();
    /**
     * save the path everytime a document is opened
     */
    void saveRecentlyOpenedFile(const QString& p);
    /**
     * write the file paths into the filesystem (save them)
     */
    void saveRecentlyOpenedFiles(QStringList& paths);
    /**
     * get the saved paths in a list of strings
     */
    QStringList getRawRecentlyOpenedFiles();
    /**
     * construct a QAction from each stored path and add them
     * into a provided menu after it is cleared
     */
    QMenu* getActionsRecentlyOpenedFiles(QMenu* menu);
    void eraseRecentlyOpenedFiles();
    /**
     * Save the theme chosen by the user
     */
    void saveTheme(QAction* ac);
    void loadTheme(QActionGroup* ag);
    void applyTheme(const QString& theme);
    /**
     * topLevel TagItems colors
     */
    void setTagItemColor(const QString& item, const QString& color);
    QHash<QString, QVariant> getTagItemColor();
    void clearColorItems();
    void setTagPinned(const QString& item);
    void setTagUnpinned(const QString& item);
    QStringList getTagPinned();
    void clearPinnedItems();
    /**
     * use the integrated editor or not
     */
    void saveUseEditor(bool use);
    bool loadUseEditor();
    bool containsUseEditor();
};   // namespace Settings

#endif
