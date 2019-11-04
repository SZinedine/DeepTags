#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QMovie>
#include <QStatusBar>
#include <QVBoxLayout>

#include "../element/element.h"
#include "elementdialog.h"
#include "readersdialog.h"
#include "settings.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupCentral();
    setupLayout();
    setupMenu();
    setupSignals();

    Settings::loadWindowSize(this);
    Settings::loadSplitterState(splitter);
    Settings::loadTheme(themesActionGroup);

    emit started();
}

void MainWindow::setupCentral() {
    auto* widget = new QWidget(this);
    setCentralWidget(widget);

    tagsContainer = new TagsContainer(this);
    tagsContainer->setHeaderHidden(true);
    filesContainer = new FilesContainer(this);
    splitter       = new QSplitter(this);
    splitter->setChildrenCollapsible(false);

    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText(tr("Search"));
    searchLineEdit->setFixedWidth(300);
    eraseSearch = searchLineEdit->addAction(QIcon(":images/quit.png"), QLineEdit::TrailingPosition);
    searchLineEdit->setContentsMargins(0, 0, 0, 0);
    eraseSearch->setVisible(false);

    expandButton = new QPushButton(QIcon(":images/expand.png"), "", this);
    expandButton->setToolTip(tr("Expand All"));
    collapseButton = new QPushButton(QIcon(":images/collapse.png"), "", this);
    collapseButton->setToolTip(tr("Collapse All"));
    expandButton->setMaximumWidth(30);
    collapseButton->setMaximumWidth(30);

    QStatusBar* statusB = statusBar();
    nbFiles             = new QLabel(tr("0 files"), this);
    statusB->addPermanentWidget(nbFiles);

    spinnerLabel = new QLabel(this);
    auto* movie  = new QMovie(this);
    movie->setFileName(":images/spinner.gif");
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

    auto* colLayout = new QVBoxLayout;
    colLayout->addWidget(collapseButton);
    colLayout->addWidget(expandButton);
    colLayout->setAlignment(Qt::AlignTop);
    colLayout->setContentsMargins(0, 0, 0, 0);
    expandButton->setContentsMargins(0, 0, 0, 0);
    collapseButton->setContentsMargins(0, 0, 0, 0);

    layout->addLayout(colLayout, 1, 0);
    layout->addWidget(splitter, 1, 1, 7, 7);
}

void MainWindow::setupMenu() {
    auto menuFile           = menuBar()->addMenu(tr("&File"));
    newFileAction           = new QAction(QIcon(":images/newFile.png"), tr("&New File"), this);
    recentlyOpenedFilesMenu = new QMenu(tr("&Recently Opened Files"), this);
    Settings::getActionsRecentlyOpenedFiles(recentlyOpenedFilesMenu);
    changeDataDirAction = new QAction(tr("&Change Data Directory"), this);
    quitAction          = new QAction(QIcon(":images/quit.png"), tr("&Quit"), this);
    menuFile->addAction(newFileAction);
    menuFile->addMenu(recentlyOpenedFilesMenu);
    menuFile->addActions({changeDataDirAction, quitAction});

    auto menuEdit     = menuBar()->addMenu(tr("&Edit"));
    setMdReaderAction = new QAction(tr("&Set MarkDown Reader"), this);

    setStyleMenu      = new QMenu(tr("Themes"), menuEdit);
    themesActionGroup = new QActionGroup(this);
    themesActionGroup->setExclusive(true);
    QAction* nativeStyleAction     = new QAction(tr("Native Style"), setStyleMenu);
    QAction* breezeDarkStyleAction = new QAction(tr("Dark Style"), setStyleMenu);
    setStyleMenu->addActions({nativeStyleAction, breezeDarkStyleAction});
    themesActionGroup->addAction(nativeStyleAction);
    themesActionGroup->addAction(breezeDarkStyleAction);
    nativeStyleAction->setData(QString("native"));
    breezeDarkStyleAction->setData(QString(":qdarkstyle/style.qss"));
    for (auto* ac : themesActionGroup->actions()) ac->setCheckable(true);
    nativeStyleAction->setChecked("true");

    clearElementsAction  = new QAction(tr("Clear Elements"));
    reloadElementsAction = new QAction(tr("Reload Elements"));

    menuEdit->addMenu(setStyleMenu);
    menuEdit->addActions({setMdReaderAction, clearElementsAction, reloadElementsAction});

    auto menuHelp = new QMenu(this);
    aboutAction   = new QAction(tr("&About"), this);
    menuHelp      = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(aboutAction);

    // append the search bar into the right of the menu bar
    auto w = new QWidget(this);
    w->setLayout(new QHBoxLayout);
    w->layout()->addWidget(searchLineEdit);
    w->setContentsMargins(0, 0, 5, 0);
    menuBar()->setCornerWidget(w);
}


void MainWindow::setupSignals() {
    connect(tagsContainer, &TagsContainer::itemSelected, filesContainer, &FilesContainer::addFiles);
    connect(tagsContainer, &TagsContainer::itemSelected, this,
            &MainWindow::changeNumberOfFilesLabel);
    connect(filesContainer, &FilesContainer::numberOfElementsChanged, this,
            &MainWindow::changeNumberOfFilesLabel);
    connect(filesContainer, &FilesContainer::elementChanged, tagsContainer,
            &TagsContainer::reloadElement);
    connect(splitter, &QSplitter::splitterMoved, this,
            [=]() { Settings::saveSplitterState(splitter); });
    connect(clearElementsAction, &QAction::triggered, tagsContainer, &TagsContainer::init);
    connect(clearElementsAction, &QAction::triggered, filesContainer, &FilesContainer::clearView);
    connect(reloadElementsAction, &QAction::triggered, this, &MainWindow::reloadContent);
    connect(searchLineEdit, &QLineEdit::textEdited, this, &MainWindow::search);
    connect(eraseSearch, &QAction::triggered, [&] {
        if ((tagsContainer->selectedItems().isEmpty()) && !(searchLineEdit->text().isEmpty()))
            filesContainer->clearView();
        searchLineEdit->clear();
        eraseSearch->setVisible(false);
    });
    connect(searchLineEdit, &QLineEdit::textEdited, eraseSearch,
            [=] { eraseSearch->setVisible(!searchLineEdit->text().isEmpty()); });
    connect(newFileAction, &QAction::triggered, this, &MainWindow::newFiles);
    connect(recentlyOpenedFilesMenu, &QMenu::triggered, Settings::openFileAction);
    connect(recentlyOpenedFilesMenu, &QMenu::triggered, this,
            [=]() { Settings::getActionsRecentlyOpenedFiles(recentlyOpenedFilesMenu); });
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(setMdReaderAction, &QAction::triggered, this,
            [=] { std::make_unique<ReadersDialog>(this); });
    connect(changeDataDirAction, &QAction::triggered, this, &MainWindow::changeDataDirectory);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(this, &MainWindow::started, this, &MainWindow::load,
            Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(expandButton, &QPushButton::clicked, tagsContainer, &TagsContainer::expandItems);
    connect(collapseButton, &QPushButton::clicked, tagsContainer, &TagsContainer::collapseItems);
    connect(filesContainer, &FilesContainer::openedFile, this,
            [=]() { Settings::getActionsRecentlyOpenedFiles(recentlyOpenedFilesMenu); });
    connect(tagsContainer, &TagsContainer::loadingFiles, this,
            [=]() { spinnerLabel->setVisible(true); });
    connect(tagsContainer, &TagsContainer::filesLoaded, spinnerLabel, &QLabel::hide);
    connect(tagsContainer, &TagsContainer::loadingFiles, this, [=]() { disableSomeWidgets(true); });
    connect(tagsContainer, &TagsContainer::filesLoaded, this, [=]() { disableSomeWidgets(false); });
    connect(filesContainer, &FilesContainer::deletedItem, tagsContainer,
            &TagsContainer::permatentlyDelete);
    connect(themesActionGroup, &QActionGroup::triggered, &Settings::saveTheme);
}


void MainWindow::changeDataDirectory() {
    bool reload = Settings::setDataDirectory();
    if (reload) reloadContent();
}

void MainWindow::disableSomeWidgets(const bool& disable) {
    changeDataDirAction->setDisabled(disable);
    reloadElementsAction->setDisabled(disable);
    clearElementsAction->setDisabled(disable);
    searchLineEdit->setDisabled(disable);
}

void MainWindow::load() {
    qApp->processEvents();
    auto op = [&](const QString& title, const QString& message) {
        auto ask =
            QMessageBox::information(this, title, message, QMessageBox::Ok | QMessageBox::Cancel);
        if (ask == QMessageBox::Cancel) exit(0);
        if (!Settings::setDataDirectory()) exit(0);
    };

    if (!Settings::dataDirectoryIsSet())
        op(tr("Set a Data Directory"), tr("The Data directory isn't set, Please set it."));
    if (Settings::dataDirectoryIsSet() && !QFile::exists(Settings::dataDirectory()))
        op(tr("Data Directory doesn't exist"),
           tr("The Data Directory doesn't exist. Plase set it"));

    loadDataDirectoryContent();
    // remember if the items are expanded/collapsed the last time
    tagsContainer->loadCollapseOrExpand();
}


QStringList MainWindow::currentPaths() const {
    QStringList paths;
    // iterate through all the TagItems,
    // get the vector containing the elements for each one,
    // extract the filepath, add it to the list
    QTreeWidgetItemIterator it(tagsContainer);
    while (*it) {
        TagItem*           item     = TagsContainer::real(*it);
        QVector<Element*>* elements = item->elements();
        for (Element* e : *elements) paths.append(QString(e->path().string().c_str()));
        it++;
    }
    paths.removeDuplicates();
    return paths;
}


void MainWindow::reloadContent() {
    tagsContainer->init();
    filesContainer->clearView();
    loadDataDirectoryContent();
}

void MainWindow::openStringListPaths(const QStringList& strlist) {
    ElementsList res;

    for (const QString& i : strlist) {
        if (!QFile::exists(i)) continue;    // would crash if the file doesn't exist anymore
        res.push_back(new Element(fs::path(i.toStdString())));
    }

    openElements(res);
}


void MainWindow::loadDataDirectoryContent() {
    const PathsList    paths    = be::fetch_files(Settings::dataDirectory().toStdString());
    const ElementsList elements = Element::constructElementList(paths);
    if (!elements.empty()) openElements(elements);
}


void MainWindow::newFiles() {
    ElementDialog* dialog = new ElementDialog(this);
    auto           out    = dialog->exec();
    if (out == ElementDialog::Rejected) return;

    Element* e = new Element(dialog->path());
    if (!e) return;
    ElementsList lst{e};
    openElements(lst);
    delete dialog;
}


void MainWindow::search() {
    QString line = searchLineEdit->text().simplified().toLower();
    filesContainer->clearView();
    tagsContainer->clearSelection();
    changeNumberOfFilesLabel();
    if (line.isEmpty()) return;

    QStringList keywords{line.split(' ')};
    for (auto& s : keywords) s = s.simplified().toLower();

    auto* lst = TagsContainer::real(tagsContainer->topLevelItem(0))->elements();
    auto* res = new QVector<Element*>();

    auto contains = [&](const QString& title) {
        for (const QString& s : keywords)
            if (!(title.contains(s))) return false;
        return true;
    };

    for (Element* e : *lst)
        if (contains(QString(e->title().c_str()).simplified().toLower())) res->push_back(e);

    filesContainer->addFiles(res);
    delete res;
}

void MainWindow::changeNumberOfFilesLabel() {
    nbFiles->setText(QString(QString::number(filesContainer->count()) + QString(" files")));
}


void MainWindow::about() {
    QString str = "";
    str.append("<br><b>Name: </b>DeepTags<br>");
    str.append("<b>Version: </b>");
    str.append(qApp->applicationVersion());
    str.append("<br><b>Author:</b> Zineddine SAIBI.<br>");
    str.append("<b>E-mail: </b> saibi.zineddine@yahoo.com<br>");
    str.append("<b>website: </b> https://github.com/SZinedine <br>");
    QMessageBox::about(this, tr("About"), str);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    Settings::saveUiSettings(size(), splitter->saveState());
    event->accept();
}
