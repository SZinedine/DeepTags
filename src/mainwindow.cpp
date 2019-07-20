#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QInputDialog>
#include <QDebug>
#include <QStatusBar>
#include <QMessageBox>
#include "back/element.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupCentral();
    setupLayout();
    setupMenu();
    setupSignals();

    openAlwaysOpeningDirs();
    loadSettings();
}

void MainWindow::setupCentral() {
    QWidget* widget = new QWidget;
    setCentralWidget(widget);

    tagsContainer = new TagsContainer;
    tagsContainer->setHeaderHidden(true);
    filesContainer = new FilesContainer;

    clearTagsButton = new QPushButton("clear", this);
    clearTagsButton->setMaximumWidth(60);
    reloadButton = new QPushButton("Reload", this);
    reloadButton->setMaximumWidth(60);

    QStatusBar* statusB = statusBar();
    nbFiles = new QLabel("0 files");
    statusB->addPermanentWidget(nbFiles);

    setMinimumSize(500, 300);
}

void MainWindow::setupLayout() {
    QVBoxLayout* layout = new QVBoxLayout;
    centralWidget()->setLayout(layout);

    // containers
    QHBoxLayout* containersLayout = new QHBoxLayout;
    containersLayout->addWidget(tagsContainer);
    containersLayout->addWidget(filesContainer);

    // widgets above the containers
    QHBoxLayout *above = new QHBoxLayout;
    above->addWidget(clearTagsButton);
    above->addWidget(reloadButton);
    above->addSpacing(200);
    above->setAlignment(Qt::AlignLeft);
    //
    layout->addLayout(above);
    layout->addLayout(containersLayout);
}

void MainWindow::setupSignals() {
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	filesContainer, 	&FilesContainer::addFiles	);
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	this, 				[=](){	changeNumberOfFilesLabel();}  );
    connect(filesContainer, 	&FilesContainer::numberOfElementsChanged,this, 		[=](){	changeNumberOfFilesLabel();}  );
    connect(clearTagsButton, 	&QPushButton::clicked, 			tagsContainer, 		&TagsContainer::init		);
    connect(clearTagsButton, 	&QPushButton::clicked, 			filesContainer,		&FilesContainer::clearView	);
    connect(reloadButton, 		&QPushButton::clicked, 			this, 				[=](){  reloadContent();}	);
    connect(loadDirAction, 		&QAction::triggered, 			this, 				[=](){  loadDir(); 		}	);
    connect(loadFileAction, 	&QAction::triggered, 			this, 				[=](){  loadFiles();	}	);
    connect(quitAction, 		&QAction::triggered, 			this, 				&QMainWindow::close	 		);
    connect(setMdReaderAction,	&QAction::triggered,			this,				[=](){askForMarkdownEditor();});
    connect(setAlwaysOpeningDirsAction, &QAction::triggered,	this,				[=](){ openDirsDialog(); }	);
    connect(aboutAction,		&QAction::triggered,			this,				&MainWindow::about			);
}

void MainWindow::setupMenu() {
    menuFile 			= new QMenu;
    menuFile = menuBar()->addMenu("File");
    loadDirAction 		= new QAction("Load Directory", this);
    loadFileAction 		= new QAction("Load a File", this);
    quitAction 			= new QAction("Quit", this);
    menuFile->addActions({loadDirAction, loadFileAction, quitAction});

    menuEdit 			= new QMenu;
    setMdReaderAction 	= new QAction("Set MarkDown Reader");
    setAlwaysOpeningDirsAction = new QAction("Set Always Opening Directories on Startup");
    menuEdit = menuBar()->addMenu("Edit");
    menuEdit->addAction(setMdReaderAction);
    menuEdit->addAction(setAlwaysOpeningDirsAction);

    menuHelp 			= new QMenu;
    aboutAction			= new QAction("About");
    menuHelp = menuBar()->addMenu("Help");
    menuHelp->addAction(aboutAction);
}


void MainWindow::loadDir() {
    QString p = QFileDialog::getExistingDirectory(this, tr("Open Directory"), getLastDir());

    if (p.isEmpty()) return;
    saveLastDir(p);		// save it for future usage
    ElementsList elements = construct_list_elements( fetch_files(p.toStdString()) );
    openElements(elements);
}


void MainWindow::loadFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, QString("Open Files"), getLastFile());
    if (files.isEmpty()) return;

    ElementsList elements;
    for (const QString& s : files) {
        fs::path p(s.toStdString());
        elements.push_back( construct_single_element(p) );
    }

    if ( elements.empty() ) return;
    openElements(elements);
    saveLastFile(files.at(0));
}


void MainWindow::writeSettings() {
    QSettings s;
    s.beginGroup("paths");
    s.setValue("filepaths", QVariant(currentPaths()) );		// save the files
    s.endGroup();

    s.beginGroup("Main");
    s.setValue("windowsize", QVariant(size()));
    s.endGroup();
}


void MainWindow::loadSettings() {

    // get the saved StringList of filepaths
    QSettings s;
    s.beginGroup("paths");
    const QStringList strl = s.value("filepaths").toStringList();
    s.endGroup();
    if (!strl.empty()) openStringListPaths(strl);

    s.beginGroup("Main");
    resize(s.value("windowsize").toSize());
    s.endGroup();
}


void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}


QStringList MainWindow::currentPaths() const {
    QStringList paths;
    // iterate through all the TagItems,
    // get the vector containing the elements for each one,
    // extract the filepath, add it to the list
    QTreeWidgetItemIterator it(tagsContainer);
    while(*it) {
        TagItem* item = TagsContainer::real(*it);
        QVector<Element*>* elements = item->elements();
        for (Element* e : *elements) paths.append( QString(e->path().c_str()) );
        it++;
    }
    paths.removeDuplicates();
    return paths;
}


void MainWindow::saveLastDir(const QString &complete) {
    QSettings s;
    s.beginGroup("Main");
    s.setValue("lastOpenedDir", QVariant(complete));
    s.endGroup();
}

QString MainWindow::getLastDir() const {
    QSettings s;
    s.beginGroup("Main");
    QString d = s.value("lastOpenedDir").toString();
    s.endGroup();
    return (d.isEmpty()) ? QDir::homePath() : d;
}



void MainWindow::saveLastFile(const QString &complete) {
    QSettings s;
    s.beginGroup("Main");
    s.setValue("DirLastOpenedFile", QVariant(complete));
    s.endGroup();
}

QString MainWindow::getLastFile() const {
    QSettings s;
    s.beginGroup("Main");
    QString d = s.value("DirLastOpenedFile").toString();
    s.endGroup();

    return (d.isEmpty()) ? QDir::homePath() : d;
}


void MainWindow::openStringListPaths(const QStringList& strlist) {
    for (const QString& i : strlist){
        if(!QFile::exists(i)) continue;			// added after the crash that happens after a non existant file is being loaded
        openElement(construct_single_element( fs::path( i.toStdString() ) ));
    }
}


void MainWindow::reloadContent() {
    QStringList lst = currentPaths();
    tagsContainer->clear();
    filesContainer->clearView();
    tagsContainer->createBasicTags();
    openStringListPaths(lst);
}


void MainWindow::askForMarkdownEditor() {
    // get the previously entered value
    QSettings ss;
    ss.beginGroup("Main");
    QString previous = ss.value("MarkDownReader").toString();
    ss.endGroup();

    // launch the dialog
    QString prog = QInputDialog::getText(this, "Markdown Editor",
                                         "Name/Path of the Markdown Reader to use \t\t",
                                         QLineEdit::Normal, previous);
    // save the new command
    if (prog.isEmpty()) return;
    QSettings s;
    s.beginGroup("Main");
    s.setValue("MarkDownReader", prog);
    s.endGroup();
}

void MainWindow::openDirsDialog() {
    OpenDirs* openDirs = new OpenDirs;
    openDirs->exec();
    delete openDirs;
}

void MainWindow::openAlwaysOpeningDirs() {

    QStringList sl = OpenDirs::savedTextList();
    for (const QString& s : sl) {
        if(!QDir(s).exists(s)) continue;
        PathsList paths = fetch_files(s.toStdString());
        ElementsList elsLst = construct_list_elements(paths);
        openElements(elsLst);
    }
}


void MainWindow::about() {
    QString str = "";
    str.append("<br><b>Name: </b>DeepTags<br>");
    str.append("<b>Version: </b>");
    str.append(qApp->applicationVersion());
    str.append("<br><b>Author:</b> Zineddine SAIBI.<br>");
    str.append("<b>E-mail: </b> saibi.zineddine@yahoo.com<br>");
    str.append("<b>website: </b> https://github.com/SZinedine <br>");
    QMessageBox::about(this, "About", str);
}
