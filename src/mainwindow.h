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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "element.h"
#ifdef INSIDE_EDITOR
    #include "editorwidget.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class QCloseEvent;
class QActionGroup;
class QSystemTrayIcon;
class QLineEdit;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    /**
     * constructor helper functions
     */
    void setupCentral();
    void setupMenu();
    void setupShortcuts();
    void setupSignals();
    /**
     *  open saved files and always openning dirs at startup
     */
    void startup();
    /**
     * launch the Settings Dialog and make the necessary changes if it is accepted
     */
    void settingsDialog();
    /**
     * when the data directory is changed
     * save the new data directory, clear all, and reload
     */
    void dataDirectoryDialog();
    /**
     *  get the data directory, fetch all the files in it,
     *  construct Element from each one of the files
     *  open them (openElements())
     */
    void loadDataDirectoryContent();
    /**
     *  reload the displayed content
     */
    void reloadContent();
    /**
     *  open ElementDialog to create a new file
     *  by asking the user for information about the file
     */
    void newFile();
    /**
     *  called everytime the user write in the search LineEdit.
     *  it searches in titles of loaded files and displays them
     */
    void search();
    /**
     * convinient function to TagsContainer::addElement()
     */
    void openElements(const ElementsList& els);
    /**
     * disable some widgets when the UI is loading elements
     * this prevents any errors from occuring
     */
    void disableSomeWidgets(const bool& disable);
    /**
     * change the number of the files displayed in the status bar
     */
    void changeNumberOfFilesLabel();
    /**
     * display a dialog with some info
     */
    void about();
    /**
     * save the state of the splitter before quitting
     */
    void closeEvent(QCloseEvent* event) override;

signals:
    /**
     * emitted when the widgets are finished loading
     * after this signal is emitted, the Elements will begin loading
     */
    void started();

private:
    Ui::MainWindow* ui;
    QLineEdit* searchLineEdit;
    QAction* eraseSearch;
    QLabel* nbFiles;
    QLabel* spinnerLabel;
    QSystemTrayIcon* systray;
    QAction* systrayExitAction;
#ifdef INCLUDE_QBREEZE
    QActionGroup* themesActionGroup;
#endif
#ifdef INSIDE_EDITOR
    EditorWidget* editorWidget;
#endif
};

#endif   // MAINWINDOW_H
