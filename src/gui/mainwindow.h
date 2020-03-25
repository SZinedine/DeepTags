#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filescontainer.h"
#include "tagscontainer.h"
#ifdef INSIDE_EDITOR
    #include "editorwidget.h"
#endif

class QAction;
class QCloseEvent;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSplitter;
class QActionGroup;
class QSystemTrayIcon;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private:
    void setupCentral();
    void setupLayout();
    void setupMenu();
    void setupSignals();
    void setupKeyboard();
    /**
     *  open saved files and always openning dirs at startup
     */
    void load();
    /**
     *  reload the displayed content
     */
    void reloadContent();
    /**
     * when the data directory is changed
     * save the new data directory, clear all, and reload
     */
    void changeDataDirectory();
    /**
     *  get the data directory, fetch all the files in it,
     *  construct Element from each one of the files
     *  open them (openElements())
     */
    void loadDataDirectoryContent();
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
    void changeNumberOfFilesLabel();
    inline void openElements(const ElementsList& els) { tagsContainer->addElements(els); }
    void about();
    void closeEvent(QCloseEvent* event) override;
    void disableSomeWidgets(const bool& disable);
    void setTheme(QAction* action);
    void loadTheme();

signals:
    void started();

private:
    TagsContainer* tagsContainer;
    FilesContainer* filesContainer;
    QSplitter* splitter;
    QLineEdit* searchLineEdit;
    QAction* eraseSearch;
    QPushButton* expandButton;
    QPushButton* collapseButton;
    QLabel* nbFiles;
    QLabel* spinnerLabel;
    QAction* newFileAction;
    QMenu* recentlyOpenedFilesMenu;
    QAction* changeDataDirAction;
    QAction* openDataDirAction;
    QAction* quitAction;
    QAction* clearElementsAction;
    QAction* reloadElementsAction;
    QAction* setMdReaderAction;
    QAction* aboutAction;
    QAction* systrayExitAction;
    QSystemTrayIcon* systray;
#ifdef INCLUDE_QBREEZE
    QActionGroup* themesActionGroup;
#endif
#ifdef INSIDE_EDITOR
    EditorWidget* editorWidget;
    QAction* editorWidgetAction;
#endif
};


#endif   // MAINWINDOW_H
