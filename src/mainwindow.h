#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QCloseEvent>
#include <QLabel>
#include <QString>
#include "tagscontainer.h"
#include "filescontainer.h"
#include "opendirs.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent=nullptr);

    void closeEvent(QCloseEvent* event);

private:
    void setupCentral();
    void setupLayout();
    void setupMenu();
    void setupSignals();
    void loadDir();								// open the files of a directory from the filesystem
    void loadFiles();							// open files from the filesystem
    void writeSettings();						// save the opened files to open in the next start
    void loadSettings();						// retrieve the list of saved files (paths) and load them into the app
    QStringList currentPaths() const;			// retrieve all the filepaths loaded
    void saveLastDir(const QString& complete);	// save the opened directory
    void saveLastFile(const QString& complete);	// save the directory of the opened file
    QString getLastDir() const;					// get the last opened directory
    QString getLastFile() const;				// get the directory of the last opened file
    void reloadContent();						// reload the displayed content
    void openStringListPaths(const QStringList& strlist);	// receives list of Qstrings of filepaths, make of them fs::path, construct Elements, then send them one by one to be displayed
    void askForMarkdownEditor();				// ask the user to input the markdown editor with which the files will be opened when clicked on
    void openDirsDialog();						// open OpenDir and ask the user for directory paths to open on every startup
    void openAlwaysOpeningDirs();				// open the directories saved by OpenDirs
    inline void openElement(Element* element)			{	tagsContainer->addElement(element);		}
    inline void openElements(const ElementsList& els)	{	tagsContainer->addElements(els);		}
    inline void changeNumberOfFilesLabel()				{	nbFiles->setText(QString( QString::number(filesContainer->count()) + QString(" files") ));					}
    void about();


private:
    TagsContainer *tagsContainer;
    FilesContainer* filesContainer;
    OpenDirs* openDirs;
    QPushButton* clearTagsButton;
    QPushButton* reloadButton;
    QLabel* nbFiles;
    QMenu* menuFile;
        QAction* loadDirAction;
        QAction* loadFileAction;
        QAction* quitAction;
    QMenu* menuEdit;
        QAction* setMdReaderAction;
        QAction* setAlwaysOpeningDirsAction;
    QMenu* menuHelp;
        QAction* aboutAction;
};


#endif // MAINWINDOW_H
