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
    void closeEvent(QCloseEvent* event) override;

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
     *  open a file from the filesystem and add it to the app
     */
    void importFile();
    /**
     *  save the size of the window and the state of the splitter
     */
    void saveUiSettings();
    void loadUiSettings();
    /**
     *  retrieve all the filepaths loaded
     */
    QStringList currentPaths() const;
    /**
     *  save the directory of the opened file
     */
    void saveLastVisitedDir(const QString& complete);
    /**
     *  get the directory of the last opened file
     */
    QString getLastVisitedDir() const;
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
     *  ask the user to input the markdown editor 
     *  with which the files will be opened when clicked on
     */
    void askForMarkdownEditor();
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
    /**
     *  displays a dialog with the author information in it
     */
    void about();
    inline void openElements(const ElementsList& els) { tagsContainer->addElements(els);}
    inline void changeNumberOfFilesLabel()        {   nbFiles->setText(QString( QString::number(filesContainer->count()) + QString(" files") ));    }
    bool setDataDirectory();
    /**
     *  check if the data directory have been set
     */
    bool dataDirectoryIsSet() const;
    /**
     *  return the data directory
     */
    QString dataDirectory() const;
    void loadDataDirectoryContent();

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
        QAction* changeDataDirAction;
        QAction* loadFileAction;
        QAction* quitAction;
    QMenu* menuEdit;
        QAction* setMdReaderAction;
    QMenu* menuHelp;
        QAction* aboutAction;
};


#endif // MAINWINDOW_H
