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
    void load();                                // open saved files and always openning dirs at startup
    void importFile();                          // open a file from the filesystem and add it to the app
    void saveUiSettings();                      // save the size of the window and the state of the splitter
    void loadUiSettings();
    QStringList currentPaths() const;           // retrieve all the filepaths loaded
    void saveLastVisitedDir(const QString& complete); // save the directory of the opened file
    QString getLastVisitedDir() const;          // get the directory of the last opened file
    void reloadContent();                       // reload the displayed content
    void openStringListPaths(const QStringList& strlist);   // receives list of Qstrings of filepaths, make of them fs::path, construct Elements, then send them one by one to be displayed
    void askForMarkdownEditor();                // ask the user to input the markdown editor with which the files will be opened when clicked on
    void newFiles();                            // open ElementDialog to create a new file by asking the user for information about the file
    void search();                              // called everytime the user write in the search LineEdit. it searches in titles of loaded files and displays them
    void about();                               // displays a dialog with the author information in it
    inline void openElements(const ElementsList& els)    {   tagsContainer->addElements(els);    }
    inline void changeNumberOfFilesLabel()        {   nbFiles->setText(QString( QString::number(filesContainer->count()) + QString(" files") ));    }
    bool setDataDirectory();
    bool dataDirectoryIsSet() const;             // check if the data directory have been set
    QString dataDirectory() const;               // return the data directory
    void loadDataDirectoryContent();

signals:
    void started();                             // emited at the en of the constructor

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
