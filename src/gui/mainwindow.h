#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QCloseEvent>
#include <QLabel>
#include <QString>
#include <QSplitter>
#include <QLineEdit>
#include "tagscontainer.h"
#include "filescontainer.h"
#include <iostream>
#include <chrono>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);

private:
    void setupCentral();
    void setupLayout();
    void setupMenu();
    void setupSignals();
    /**
     *  open saved files and always openning dirs at startup
     */
    void load();
    /**
     *  retrieve all the filepaths loaded
     */
    QStringList currentPaths() const;
    /**
     *  reload the displayed content
     */
    void reloadContent();
    /**
     *  receives list of Qstrings of filepaths, 
     *  make of them fs::path, construct Elements, 
     *  then send them one by one to be displayed
     */
    void openStringListPaths(const QStringList& strlist);
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
    void newFiles();
    /**
     *  called everytime the user write in the search LineEdit. 
     *  it searches in titles of loaded files and displays them
     */
    void search();
    void changeNumberOfFilesLabel();
    inline void openElements(const ElementsList& els) { tagsContainer->addElements(els);}
    void about();
    void closeEvent(QCloseEvent* event) override;

    void disableSomeWidgets(const bool& disable);

signals:
    /**
     *  emited at the en of the constructor
     */
    void started();

private:
    TagsContainer *tagsContainer;
    FilesContainer* filesContainer;
    QSplitter* splitter;
    QPushButton* clearTagsButton;
    QPushButton* reloadButton;
    QLineEdit* searchLineEdit;
    QPushButton* expandButton;
    QPushButton* collapseButton;
    QLabel* nbFiles;
    QLabel* spinnerLabel;
    QMenu* menuFile;
        QAction* newFileAction;
        QMenu* recentlyOpenedFilesMenu;
        QAction* changeDataDirAction;
        QAction* quitAction;
    QMenu* menuEdit;
        QAction* setMdReaderAction;
    QMenu* menuHelp;
        QAction* aboutAction;
};


#endif // MAINWINDOW_H
