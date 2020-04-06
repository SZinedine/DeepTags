#include "mainwindow.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMovie>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QStatusBar>
#include <QSystemTrayIcon>
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
    setupKeyboard();

    Settings::loadWindowSize(this);
    Settings::loadSplitterState(splitter);
#ifdef INCLUDE_QBREEZE   // use QBreeze if it exists
    Settings::loadTheme(themesActionGroup);
#endif
#ifdef INSIDE_EDITOR
    editorWidgetAction->setChecked(Settings::loadUseEditor());
#endif
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

    systray = new QSystemTrayIcon(QIcon(":images/icon128.png"), this);
    systray->show();

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
    auto layout = new QGridLayout;
    centralWidget()->setLayout(layout);

    // containers
    splitter->addWidget(tagsContainer);
    splitter->addWidget(filesContainer);
#ifdef INSIDE_EDITOR
    editorWidget = new EditorWidget(this);
    splitter->addWidget(editorWidget);
#endif

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
    openDataDirAction   = new QAction(tr("Open Data Directory"), this);
    quitAction          = new QAction(QIcon(":images/quit.png"), tr("&Quit"), this);
    menuFile->addAction(newFileAction);
    menuFile->addMenu(recentlyOpenedFilesMenu);
    menuFile->addAction(changeDataDirAction);
    menuFile->addAction(openDataDirAction);
    menuFile->addSeparator();
    menuFile->addAction(quitAction);

    auto menuEdit     = menuBar()->addMenu(tr("&Edit"));
    setMdReaderAction = new QAction(tr("&Set MarkDown Reader"), this);
#ifdef INCLUDE_QBREEZE   // use QBreeze if it exists
    auto setStyleMenu = new QMenu(tr("Themes"), menuEdit);
    themesActionGroup = new QActionGroup(this);
    themesActionGroup->setExclusive(true);
    auto nativeStyleAction      = new QAction(tr("Native Style"), setStyleMenu);
    auto breezeDarkStyleAction  = new QAction(tr("Dark Style"), setStyleMenu);
    auto breezeLightStyleAction = new QAction(tr("Light Style"), setStyleMenu);
    setStyleMenu->addActions({ nativeStyleAction, breezeDarkStyleAction, breezeLightStyleAction });
    for (auto& ac : { nativeStyleAction, breezeDarkStyleAction, breezeLightStyleAction })
        themesActionGroup->addAction(ac);
    nativeStyleAction->setData(QString("native"));
    breezeDarkStyleAction->setData(QString(":qbreeze/dark.qss"));
    breezeLightStyleAction->setData(QString(":qbreeze/light.qss"));
    for (auto* ac : themesActionGroup->actions()) ac->setCheckable(true);
    nativeStyleAction->setChecked(true);
    menuEdit->addMenu(setStyleMenu);
#endif
    clearElementsAction  = new QAction(tr("Clear Elements"), menuEdit);
    reloadElementsAction = new QAction(tr("Reload Elements"), menuEdit);

    menuEdit->addActions({ setMdReaderAction, clearElementsAction, reloadElementsAction });

#ifdef INSIDE_EDITOR
    editorWidgetAction = new QAction("Activate Integrated Reader (experimental)", menuEdit);
    editorWidgetAction->setCheckable(true);
    editorWidgetAction->setChecked(false);
    menuEdit->addAction(editorWidgetAction);
#endif

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

    // systray menu
    systrayExitAction = new QAction("Exit", this);
    auto systrayMenu  = new QMenu(this);
    systrayMenu->addAction(systrayExitAction);
    systray->setContextMenu(systrayMenu);
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
    connect(newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(recentlyOpenedFilesMenu, &QMenu::triggered, Settings::openFileAction);
    connect(recentlyOpenedFilesMenu, &QMenu::triggered, this,
            [=]() { Settings::getActionsRecentlyOpenedFiles(recentlyOpenedFilesMenu); });
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(setMdReaderAction, &QAction::triggered, this,
            [=] { std::make_unique<ReadersDialog>(this); });
    connect(changeDataDirAction, &QAction::triggered, this, &MainWindow::changeDataDirectory);
    connect(openDataDirAction, &QAction::triggered, this,
            [=] { QDesktopServices::openUrl(QUrl(Settings::dataDirectory())); });
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
    connect(systray, &QSystemTrayIcon::activated, this, [=] { setVisible(!isVisible()); });
    connect(systrayExitAction, &QAction::triggered, this, [=] {
        systray->hide();
        close();
    });
#ifdef INCLUDE_QBREEZE
    connect(themesActionGroup, &QActionGroup::triggered, &Settings::saveTheme);
#endif
#ifdef INSIDE_EDITOR
    // connect(filesContainer, &FilesContainer::openedFile, editorWidget, &EditorWidget::closeFile);
    connect(filesContainer, &FilesContainer::elementChanged, editorWidget, &EditorWidget::reload);
    connect(editorWidgetAction, &QAction::toggled, this, [=] {
        bool checked = editorWidgetAction->isChecked();
        Settings::saveUseEditor(checked);
        if (checked)
            connect(filesContainer, &FilesContainer::selectionChanged_, editorWidget,
                    &EditorWidget::open);
        else {
            disconnect(filesContainer, &FilesContainer::selectionChanged_, editorWidget,
                       &EditorWidget::open);
            editorWidget->closeFile();
        }
    });
#endif
}


void MainWindow::setupKeyboard() {
    auto srch = new QShortcut(QKeySequence("Ctrl+f"), this);
    connect(srch, &QShortcut::activated, this, [=] { searchLineEdit->setFocus(); });
    newFileAction->setShortcut(QKeySequence("Ctrl+n"));
    quitAction->setShortcut(QKeySequence("Ctrl+q"));
    // clearElementsAction->setShortcut(QKeySequence("Ctrl+Shift+c"));
    // reloadElementsAction->setShortcut(QKeySequence("Ctrl+Shift+r"));
    auto ret = new QShortcut(QKeySequence("Return"), this);
    connect(ret, &QShortcut::activated, this, [=] {
        if (tagsContainer->hasFocus())
            tagsContainer->selected();
        else if (filesContainer->hasFocus())
            filesContainer->openCurrent();
        else if (searchLineEdit->hasFocus())
            search();
    });
    auto edel = new QShortcut(QKeySequence("Ctrl+e"), this);
    connect(edel, &QShortcut::activated, this, [=] {
        if (filesContainer->hasFocus()) filesContainer->editElement(filesContainer->currentItem());
    });
    auto p = new QShortcut(QKeySequence("Ctrl+p"), this);
    connect(p, &QShortcut::activated, filesContainer, &FilesContainer::pinSelected);
    auto s = new QShortcut(QKeySequence("Ctrl+s"), this);
    connect(s, &QShortcut::activated, filesContainer, &FilesContainer::starSelected);
    auto supr = new QShortcut(QKeySequence(QKeySequence::Delete), this);
    connect(supr, &QShortcut::activated, filesContainer, &FilesContainer::trashSelected);
    auto permDel = new QShortcut(QKeySequence(QKeySequence(Qt::Key_Shift + Qt::Key_Delete)), this);
    connect(permDel, &QShortcut::activated, filesContainer,
            &FilesContainer::permanentlyDeleteSelected);
    auto r = new QShortcut(QKeySequence("Ctrl+r"), this);
    connect(r, &QShortcut::activated, filesContainer, &FilesContainer::restoreSelected);
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
    if (!Settings::dataDirectoryIsSet() || !fs::exists(Settings::dataDirectory().toStdString())) {
        auto ask = QMessageBox::information(this, tr("Set a Data Directory"),
                                            tr("The Data directory isn't set, Please set it."),
                                            QMessageBox::Ok | QMessageBox::Cancel);
        if (ask == QMessageBox::Cancel) return;
        if (!Settings::setDataDirectory()) return;
    }
    loadDataDirectoryContent();
    tagsContainer->loadCollapseOrExpand();
}


void MainWindow::reloadContent() {
    tagsContainer->init();
    filesContainer->clearView();
    loadDataDirectoryContent();
}


void MainWindow::loadDataDirectoryContent() {
    const PathsList paths       = be::fetch_files(Settings::dataDirectory().toStdString());
    const ElementsList elements = Element::constructElementList(paths);
    if (!elements.empty()) openElements(elements);
}


void MainWindow::newFile() {
    if (!Settings::dataDirectoryIsSet()) {
        QMessageBox::information(this, tr("Data Directory is not set"),
                                 tr("You have to set a Data Directory before creating a file"));
        return;
    }
    auto dialog = std::make_unique<ElementDialog>(this);
    auto out    = dialog->exec();
    if (out == ElementDialog::Rejected) return;

    auto e = new Element(dialog->path());
    if (!e) return;
    ElementsList lst{ e };
    openElements(lst);
}


void MainWindow::search() {
    QString line = searchLineEdit->text().simplified().toLower();
    filesContainer->clearView();
    tagsContainer->clearSelection();
    changeNumberOfFilesLabel();
    if (line.isEmpty()) return;

    QStringList keywords{ line.split(' ') };
    for (auto& s : keywords) s = s.simplified().toLower();

    auto* lst = TagsContainer::real(tagsContainer->topLevelItem(0))->elements();
    auto res  = std::make_unique<QVector<Element*>>();

    auto contains = [&](const QString& title) {
        for (const QString& s : keywords)
            if (!(title.contains(s))) return false;
        return true;
    };

    for (Element* e : *lst)
        if (contains(QString(e->title().c_str()).simplified().toLower())) res->push_back(e);

    filesContainer->addFiles(res.get());
}

void MainWindow::changeNumberOfFilesLabel() {
    nbFiles->setText(QString(QString::number(filesContainer->count()) + QString(" files")));
}


void MainWindow::about() {
    QString str = "";
    str.append("<br><b>Name: </b>DeepTags<br>");
    str.append("<b>Version: </b>");
    str.append(qApp->applicationVersion());
    str.append(" (compiled in " + QString(__DATE__) + " at " + QString(__TIME__) + ")");
    str.append("<br><b>Author:</b> Zineddine SAIBI.<br>");
    str.append("<b>E-mail: </b> saibi.zineddine@yahoo.com<br>");
    str.append("<b>website: </b> https://github.com/SZinedine/DeepTags <br>");
    QMessageBox::about(this, tr("About"), str);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    Settings::saveUiSettings(size(), splitter->saveState());
    event->accept();
}
