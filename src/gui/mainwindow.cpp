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
#include "opendirs.h"
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
    loadDirAction 		= new QAction(QIcon(":images/addFolder.png"), tr("Import Directory"), this);
    loadFileAction 		= new QAction(QIcon(":images/addFile.png"), tr("Import File"), this);
    quitAction 			= new QAction(QIcon(":images/quit.png"), tr("&Quit"), this);
    menuFile->addActions({newFileAction, loadDirAction, loadFileAction, quitAction});

    menuEdit 			= new QMenu;
    setMdReaderAction 	= new QAction(tr("Set MarkDown Reader"));
    setAlwaysOpeningDirsAction = new QAction(tr("Set Always Opening Directories on Startup"));
    menuEdit = menuBar()->addMenu("Edit");
    menuEdit->addAction(setMdReaderAction);
    menuEdit->addAction(setAlwaysOpeningDirsAction);

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
    connect(loadDirAction, 		&QAction::triggered, 			this, 				[=](){  loadDir(); 		}	);
    connect(loadFileAction, 	&QAction::triggered, 			this, 				[=](){  loadFiles();	}	);
    connect(quitAction, 		&QAction::triggered, 			this, 				&QMainWindow::close	 		);
    connect(setMdReaderAction,	&QAction::triggered,			this,				[=](){askForMarkdownEditor();});
    connect(setAlwaysOpeningDirsAction, &QAction::triggered,	this,				[=](){ openDirsDialog(); }	);
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
        loadDirAction->setDisabled(true);
        loadFileAction->setDisabled(true);
        setAlwaysOpeningDirsAction->setDisabled(true);
    });
    connect(tagsContainer, 		&TagsContainer::filesLoaded,	this,
            [=](){
        reloadButton->setDisabled(false);
        loadDirAction->setDisabled(false);
        loadFileAction->setDisabled(false);
        setAlwaysOpeningDirsAction->setDisabled(false);
    });
}


void MainWindow::load() {
    qApp->processEvents();
    openAlwaysOpeningDirs();
    loadOpenedFiles();
    tagsContainer->loadCollapseOrExpand();		// remember if the items are expanded/collapsed the last time
}

void MainWindow::loadDir() {
    QString p = QFileDialog::getExistingDirectory(this, tr("Open Directory"), getLastDir());

    if (p.isEmpty()) return;
    saveLastDir(p);		// save it for future usage
    ElementsList elements = Element::construct_list_elements( Element::fetch_files(p.toStdString()) );

    openElements(elements);
}


void MainWindow::loadFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Files"), getLastFile());
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

void MainWindow::loadOpenedFiles() {

    // get the saved StringList of filepaths
    QSettings s;
    s.beginGroup("paths");
    const QStringList strl = s.value("filepaths").toStringList();
    s.endGroup();
    if (!strl.empty()) openStringListPaths(strl);
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


void MainWindow::saveLastDir(const QString &complete) {
    QSettings s;
    s.beginGroup("Main");
    s.setValue("lastOpenedDir", QVariant(complete));
    s.endGroup();
}



void MainWindow::saveLastFile(const QString &complete) {
    QSettings s;
    s.beginGroup("Main");
    s.setValue("DirLastOpenedFile", QVariant(complete));
    s.endGroup();
}


QString MainWindow::getLastDir() const {
    QSettings s;
    s.beginGroup("Main");
    QString d = s.value("lastOpenedDir").toString();
    s.endGroup();
    return (d.isEmpty()) ? QDir::homePath() : d;
}


QString MainWindow::getLastFile() const {
    QSettings s;
    s.beginGroup("Main");
    QString d = s.value("DirLastOpenedFile").toString();
    s.endGroup();

    return (d.isEmpty()) ? QDir::homePath() : d;
}



void MainWindow::reloadContent() {
    QStringList lst = currentPaths();
    tagsContainer->clear();
    filesContainer->clearView();
    tagsContainer->createBasicTags();
    openStringListPaths(lst);
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
    ss.beginGroup("Main");
    QString previous = ss.value("MarkDownReader").toString();
    ss.endGroup();

    // launch the dialog
    QString prog = QInputDialog::getText(this, tr("Markdown Reader"),
                                         tr("Name/Path of the Markdown Reader to use") + "\t\t",
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
    saveOpenedFiles();
    saveUiSettings();
    event->accept();
}

