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
#include "../element/element.h"
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupCentral();
    setupLayout();
    setupMenu();
    setupSignals();

    loadUiSettings();

    emit started();
}

void MainWindow::setupCentral() {
    QWidget* widget = new QWidget;
    setCentralWidget(widget);

    tagsContainer = new TagsContainer;
    tagsContainer->setHeaderHidden(true);
    filesContainer = new FilesContainer;
    splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);

    clearTagsButton = new QPushButton("clear", this);
    clearTagsButton->setMaximumWidth(60);
    reloadButton = new QPushButton("Reload", this);
    reloadButton->setMaximumWidth(60);

    expandButton = new QPushButton("exp");
    collapseButton = new QPushButton("col");

    QStatusBar* statusB = statusBar();
    nbFiles = new QLabel("0 files");
    statusB->addPermanentWidget(nbFiles);

    setMinimumSize(500, 300);
}

void MainWindow::setupLayout() {
    QVBoxLayout* layout = new QVBoxLayout;
    centralWidget()->setLayout(layout);

    // containers
    splitter->addWidget(tagsContainer);
    splitter->addWidget(filesContainer);

    QVBoxLayout* colLayout = new QVBoxLayout;
    colLayout->addWidget(expandButton);
    colLayout->addWidget(collapseButton);
    colLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout *views = new QHBoxLayout;
    views->addLayout(colLayout);
    views->addWidget(splitter);

    // widgets above the containers
    QHBoxLayout *above = new QHBoxLayout;
    above->addWidget(clearTagsButton);
    above->addWidget(reloadButton);
    above->addSpacing(200);
    above->setAlignment(Qt::AlignLeft);
    //
    layout->addLayout(above);
//    layout->addLayout(colLayout);
    layout->addWidget(splitter);
}

void MainWindow::setupSignals() {
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	filesContainer, 	&FilesContainer::addFiles	);
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	this, 				[=](){	changeNumberOfFilesLabel();	}  	);
    connect(filesContainer, 	&FilesContainer::numberOfElementsChanged,this, 		[=](){	changeNumberOfFilesLabel();	}  	);
    connect(filesContainer, 	&FilesContainer::removedItem,	tagsContainer, 		&TagsContainer::removeElement);
    connect(splitter,			&QSplitter::splitterMoved,		this,				[=](){	saveUiSettings();});
    connect(clearTagsButton, 	&QPushButton::clicked, 			tagsContainer, 		&TagsContainer::init		);
    connect(clearTagsButton, 	&QPushButton::clicked, 			filesContainer,		&FilesContainer::clearView	);
    connect(reloadButton, 		&QPushButton::clicked, 			this, 				[=](){  reloadContent();}	);
    connect(loadDirAction, 		&QAction::triggered, 			this, 				[=](){  loadDir(); 		}	);
    connect(loadFileAction, 	&QAction::triggered, 			this, 				[=](){  loadFiles();	}	);
    connect(quitAction, 		&QAction::triggered, 			this, 				&QMainWindow::close	 		);
    connect(setMdReaderAction,	&QAction::triggered,			this,				[=](){askForMarkdownEditor();});
    connect(setAlwaysOpeningDirsAction, &QAction::triggered,	this,				[=](){ openDirsDialog(); }	);
    connect(aboutAction,		&QAction::triggered,			this,				&MainWindow::about			);
    connect(this,				&MainWindow::started,			this,				&MainWindow::load,		Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection) );
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


void MainWindow::load() {
    qApp->processEvents();
    openAlwaysOpeningDirs();
    loadOpenedFiles();
}

void MainWindow::loadDir() {
    QString p = QFileDialog::getExistingDirectory(this, tr("Open Directory"), getLastDir());

    if (p.isEmpty()) return;
    saveLastDir(p);		// save it for future usage
//    ElementsList elements = construct_list_elements( fetch_files(p.toStdString()) );
    ElementsList elements = Element::construct_list_elements( Element::fetch_files(p.toStdString()) );


    openElements(elements);
}


void MainWindow::loadFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, QString("Open Files"), getLastFile());
    if (files.isEmpty()) return;

    ElementsList elements;
    for (const QString& s : files) {
        fs::path p(s.toStdString());
        if (fs::exists(p))
            elements.push_back( new Element(p) );
    }

    if ( elements.empty() ) return;
    openElements(elements);
    saveLastFile(files.at(0));
}


void MainWindow::saveOpenedFiles() {
    QSettings s;
    s.beginGroup("paths");
    s.setValue("filepaths", QVariant(currentPaths()) );		// save the files
    s.endGroup();

}

void MainWindow::saveUiSettings() {
    QSettings s;
    s.beginGroup("Main");
    s.setValue("windowsize", QVariant(size()));				// size of the window
    s.setValue("splitterSizes", splitter->saveState());
    s.endGroup();
}

void MainWindow::loadUiSettings() {
    QSettings s;
    s.beginGroup("Main");
    splitter->restoreState(s.value("splitterSizes").toByteArray());
    resize(s.value("windowsize").toSize());
    s.endGroup();
}

void MainWindow::loadOpenedFiles() {

    // get the saved StringList of filepaths
    QSettings s;
    s.beginGroup("paths");
    const QStringList strl = s.value("filepaths").toStringList();
    s.endGroup();
    if (!strl.empty()) openStringListPaths(strl);
}


void MainWindow::closeEvent(QCloseEvent *event) {
    saveOpenedFiles();
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
    ElementsList res;

    for (const QString& i : strlist) {
        if(!QFile::exists(i)) continue;			// added after the crash that happens after a non existant file is being loaded
        res.push_back(new Element( fs::path( i.toStdString() ) ));
    }

    openElements(res);
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
    int res = openDirs->exec();
    if (res == 1) openAlwaysOpeningDirs();
    delete openDirs;
}

void MainWindow::openAlwaysOpeningDirs() {

    ElementsList list;
    QStringList sl = OpenDirs::savedTextList();
    for (const QString& s : sl) {		// iterate through all the dirs, fetch the files and put them in a single container
        if(!QDir(s).exists(s)) continue;
        PathsList paths = Element::fetch_files(s.toStdString());
        ElementsList el = Element::construct_list_elements(paths);
        list.insert(list.end(), el.begin(), el.end());
    }
    openElements(list);			// call openElements only once.
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

