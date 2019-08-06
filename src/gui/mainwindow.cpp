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
#include <QGridLayout>
#include <QMovie>
#include "../element/element.h"
#include "elementdialog.h"

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
    auto* widget = new QWidget;
    setCentralWidget(widget);

    tagsContainer = new TagsContainer;
    tagsContainer->setHeaderHidden(true);
    filesContainer = new FilesContainer;
    splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);

    clearTagsButton = new QPushButton(tr("clear"), this);
    clearTagsButton->setToolTip(tr("Clear All files"));
    clearTagsButton->setMaximumWidth(60);
    reloadButton = new QPushButton(tr("Reload"), this);
    reloadButton->setToolTip(tr("Reload current files"));
    reloadButton->setMaximumWidth(60);

    searchLineEdit = new QLineEdit;
    searchLineEdit->setMaximumWidth(200);
    searchLineEdit->setPlaceholderText(tr("Search"));

    expandButton = new QPushButton(QIcon(":images/expand.png"), "", this);
    expandButton->setToolTip(tr("Expand All"));
    collapseButton = new QPushButton(QIcon(":images/collapse.png"), "", this);
    collapseButton->setToolTip(tr("Collapse All"));
    expandButton->setMaximumWidth(30);
    collapseButton->setMaximumWidth(30);

    QStatusBar* statusB = statusBar();
    nbFiles = new QLabel("0 files");
    statusB->addPermanentWidget(nbFiles);

    spinnerLabel = new QLabel;
    auto *movie = new QMovie(":images/spinner.gif");
    spinnerLabel->setMovie(movie);
    movie->setScaledSize(QSize(20, 20));
    movie->start();
    statusB->addPermanentWidget(spinnerLabel);
    spinnerLabel->setVisible(false);

    setMinimumSize(500, 300);
}

void MainWindow::setupLayout() {
    QGridLayout* layout = new QGridLayout;
    centralWidget()->setLayout(layout);

    // containers
    splitter->addWidget(tagsContainer);
    splitter->addWidget(filesContainer);

    auto *colLayout = new QVBoxLayout;
    colLayout->addWidget(expandButton);
    colLayout->addWidget(collapseButton);
    colLayout->setAlignment(Qt::AlignTop);
    colLayout->setContentsMargins(0, 0, 0, 0);
    expandButton->setContentsMargins(0, 0, 0, 0);
    collapseButton->setContentsMargins(0, 0, 0, 0);

    // widgets above the containers
    auto *above = new QHBoxLayout;
    above->addWidget(clearTagsButton);
    above->addWidget(reloadButton);
    above->setAlignment(Qt::AlignLeft);

    auto *srchLayout = new QHBoxLayout;
    srchLayout->addWidget(searchLineEdit);
    srchLayout->setAlignment(Qt::AlignRight);
    above->addLayout(srchLayout);

    //
    layout->addLayout(above, 0, 1, Qt::AlignLeft);
    layout->addLayout(colLayout, 1, 0);
    layout->addWidget(splitter, 1, 1, 7, 7);
}

void MainWindow::setupMenu() {
    menuFile 			= new QMenu;
    menuFile = menuBar()->addMenu(tr("&File"));
    newFileAction		= new QAction(QIcon(":images/newFile.png"), tr("&New File"), this);
    changeDataDirAction = new QAction(tr("Change Data Directory"));
    loadFileAction 		= new QAction(QIcon(":images/addFile.png"), tr("Import File"), this);
    quitAction 			= new QAction(QIcon(":images/quit.png"), tr("&Quit"), this);
    menuFile->addActions({newFileAction, loadFileAction, changeDataDirAction, quitAction});

    menuEdit 			= new QMenu;
    setMdReaderAction 	= new QAction(tr("Set MarkDown Reader"));
    menuEdit = menuBar()->addMenu("Edit");
    menuEdit->addAction(setMdReaderAction);

    menuHelp 			= new QMenu;
    aboutAction			= new QAction(tr("&About"));
    menuHelp = menuBar()->addMenu(tr("Help"));
    menuHelp->addAction(aboutAction);
}


void MainWindow::setupSignals() {
    // when the current tag is removed from a file, the vie
    // it is important that this function remains in this place
    // commented because weird things happen
//    connect(filesContainer,		&FilesContainer::elementChanged,this,				[=](){
//        auto elements = tagsContainer->real(tagsContainer->currentItem())->elements();
//        filesContainer->addFiles(elements);
//    } );
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	filesContainer, 	&FilesContainer::addFiles	);
    connect(tagsContainer, 		&TagsContainer::itemSelected, 	this, 				[=](){	changeNumberOfFilesLabel();	}  	);
    connect(filesContainer, 	&FilesContainer::numberOfElementsChanged,this, 		[=](){	changeNumberOfFilesLabel();	}  	);
    connect(filesContainer, 	&FilesContainer::removedItem,	tagsContainer, 		&TagsContainer::removeElement);
    connect(filesContainer,		&FilesContainer::elementChanged,tagsContainer,		&TagsContainer::reloadElement);
    connect(splitter,			&QSplitter::splitterMoved,		this,				[=](){	saveUiSettings();});
    connect(clearTagsButton, 	&QPushButton::clicked, 			tagsContainer, 		&TagsContainer::init		);
    connect(clearTagsButton, 	&QPushButton::clicked, 			filesContainer,		&FilesContainer::clearView	);
    connect(reloadButton, 		&QPushButton::clicked, 			this, 				[=](){  reloadContent();}	);
    connect(searchLineEdit, 	&QLineEdit::textEdited,			this, 				[=](){  search();}	);
    connect(newFileAction, 		&QAction::triggered, 			this, 				[=](){  newFiles();		}	);
    connect(loadFileAction, 	&QAction::triggered, 			this, 				[=](){  importFile();	}	);
    connect(quitAction, 		&QAction::triggered, 			this, 				&QMainWindow::close	 		);
    connect(setMdReaderAction,	&QAction::triggered,			this,				[=](){askForMarkdownEditor();});
    connect(changeDataDirAction, &QAction::triggered,	this,						[=](){ setDataDirectory(); }	);
    connect(aboutAction,		&QAction::triggered,			this,				&MainWindow::about			);
    connect(this,				&MainWindow::started,			this,				&MainWindow::load,
            Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection) );
    connect(expandButton,		&QPushButton::clicked,			tagsContainer,		&TagsContainer::expandItems	);
    connect(collapseButton,		&QPushButton::clicked,			tagsContainer,		&TagsContainer::collapseItems);
    // show the spinner in the status bar when the files are loading
    connect(tagsContainer,		&TagsContainer::loadingFiles,	this,				[=](){  spinnerLabel->setVisible(true);});
    connect(tagsContainer,		&TagsContainer::filesLoaded,	spinnerLabel,		&QLabel::hide				);
    // enable and disable some buttons and actions when files are loading
    connect(tagsContainer, 		&TagsContainer::loadingFiles,	this,
            [=](){
        reloadButton->setDisabled(true);
        loadFileAction->setDisabled(true);
        changeDataDirAction->setDisabled(true);
    });
    connect(tagsContainer, 		&TagsContainer::filesLoaded,	this,
            [=](){
        reloadButton->setDisabled(false);
        loadFileAction->setDisabled(false);
        changeDataDirAction->setDisabled(false);
    });
}


void MainWindow::load() {
    qApp->processEvents();

    if (!dataDirectoryIsSet()) {
        auto ask = QMessageBox::information(this, "Set a Data Directory",
                                            "The data directory isn't set. Please set it",
                                            QMessageBox::Ok | QMessageBox::Cancel);
        if (ask == QMessageBox::Cancel) exit(0);
        if (!setDataDirectory()) exit(0);
    }

    loadDataDirectoryContent();
    tagsContainer->loadCollapseOrExpand();		// remember if the items are expanded/collapsed the last time
}



void MainWindow::importFile() {
    const fs::path data_dir(dataDirectory().toStdString());
    const QString file = QFileDialog::getOpenFileName(this, tr("Open Files"), getLastVisitedDir());
    if (file.isEmpty()) return;
    saveLastVisitedDir(QFileInfo(file).path());	// open this location the next time

    const fs::path fsFile(file.toStdString());
    if (fsFile.parent_path() == data_dir) {	// inside the data dir
        QMessageBox::critical(this, "File inside ",
                              "This file is already inside your Data Directory.");
        return;
    }

    if (!Element::isMD(fsFile)) {			// isn't a markdown file
        QMessageBox::critical(this, "Invalid file",
                              "Error. This is not a Markdown file.");
        return;
    }

    int i = 0;		// if the file exists, add a number in the end of the file
    fs::path dest("");
    do {
        std::string add = " (" + QString::number(i).toStdString() + ")";
        std::string filename = fsFile.stem().string()
                + ( ( add == " (0)" ) ? "" : add ) 		// if first time (0), add ampty str
                + fsFile.extension().string();

        dest = data_dir / fs::path(filename);
        i++;
    }
    while (fs::exists(dest));


    try {
        fs::copy(fsFile, dest);
    }
    catch (fs::filesystem_error& err) {
        QMessageBox::critical(this, "Filesystem Error", QString(err.what()));
        return;
    }

    if (!Element::hasHeader(dest))	    // add a tag if it doesn't have one
        Element::createHeader(dest, dest.stem().string());	// filename as a title header item
    ElementsList elements;
    elements.push_back(new Element(dest));
    openElements(elements);
}


void MainWindow::saveUiSettings() {
    QSettings s;
    s.beginGroup("main");
    s.setValue("window_size", QVariant(size()));				// size of the window
    s.setValue("splitter_size", splitter->saveState());
    s.endGroup();
}

void MainWindow::loadUiSettings() {
    QSettings s;
    s.beginGroup("main");
    splitter->restoreState(s.value("splitter_size").toByteArray());
    resize(s.value("window_size").toSize());
    s.endGroup();
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
        for (Element* e : *elements) paths.append( QString(e->path().string().c_str()) );
        it++;
    }
    paths.removeDuplicates();
    return paths;
}



void MainWindow::saveLastVisitedDir(const QString &complete) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("last_dir", QVariant(complete));
    s.endGroup();
}



QString MainWindow::getLastVisitedDir() const {
    QSettings s;
    s.beginGroup("main");
    QString d = s.value("last_dir").toString();
    s.endGroup();

    return (d.isEmpty()) ? QDir::homePath() : d;
}



void MainWindow::reloadContent() {
    tagsContainer->clear();
    filesContainer->clearView();
    tagsContainer->createBasicTags();
    loadDataDirectoryContent();
}

void MainWindow::openStringListPaths(const QStringList& strlist) {
    ElementsList res;

    for (const QString& i : strlist) {
        if(!QFile::exists(i)) continue;			// added after the crash that happens after a non existant file is being loaded
        res.push_back(new Element( fs::path( i.toStdString() ) ));
    }

    openElements(res);
}


void MainWindow::askForMarkdownEditor() {
    // get the previously entered value
    QSettings ss;
    ss.beginGroup("main");
    QString previous = ss.value("markdown_reader").toString();
    ss.endGroup();

    // launch the dialog
    QString prog = QInputDialog::getText(this, tr("Markdown Reader"),
                                         tr("Name/Path of the Markdown Reader to use") + "\t\t",
                                         QLineEdit::Normal, previous);
    // save the new command
    if (prog.isEmpty()) return;
    QSettings s;
    s.beginGroup("main");
    s.setValue("markdown_reader", prog);
    s.endGroup();
}




bool MainWindow::setDataDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), dataDirectory());
    if (dir.isEmpty()) return false;

    QSettings s;
    s.beginGroup("main");
    s.setValue("data_dir", dir);
    if (!s.contains("last_dir"))
        s.setValue("last_dir", dir);
    s.endGroup();
    return true;
}


bool MainWindow::dataDirectoryIsSet() const {
    QSettings s;
    s.beginGroup("main");
    bool res = s.contains("data_dir");
    s.endGroup();
    return res;
}


void MainWindow::loadDataDirectoryContent() {
    const PathsList paths = Element::fetch_files(dataDirectory().toStdString());
    const ElementsList elements = Element::construct_list_elements(paths);
    if (!elements.empty())
        openElements(elements);
}


QString MainWindow::dataDirectory() const {
    if (!dataDirectoryIsSet())
        return QDir::homePath();
    QSettings s;
    s.beginGroup("main");
    QString dir = s.value("data_dir").toString();
    s.endGroup();

    return dir;
}

void MainWindow::newFiles() {
    ElementDialog* dialog = new ElementDialog(this);
    auto out = dialog->exec();
    if (out == ElementDialog::Rejected) return;

    Element * e = new Element(dialog->path());
    ElementsList lst;
    if (!e) return;
    lst.push_back(e);
    openElements(lst);
    delete dialog;
}


void MainWindow::search() {
    auto lower = [](const std::string &s) { 	// to lower case
        return QString::fromStdString(s).toLower().toStdString();
    };
    filesContainer->clear();
    if (searchLineEdit->text().isEmpty())
        return;
    tagsContainer->clearSelection();

    std::string keyword = searchLineEdit->text().toLower().toStdString();
    auto* lst = TagsContainer::real( tagsContainer->topLevelItem(0) )->elements();
    auto *res = new QVector<Element*>();

    for (Element* e : *lst)	// search the "All Notes" elements' titles to find matches for the keyword
        if (lower( e->title() ).find(keyword) != std::string::npos )
            res->push_back(e);

    filesContainer->addFiles(res);		// display the results
    delete res;
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



void MainWindow::closeEvent(QCloseEvent *event) {
    saveUiSettings();
    event->accept();
}

