/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2020  Zineddine Saibi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/
#include "mainwindow.h"
#include <QCloseEvent>
#include <QDesktopServices>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMovie>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <memory>
#include "datadirdialog.h"
#include "elementdialog.h"
#include "readersdialog.h"
#include "settings.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    Settings::loadWindowSize(this);
    setupCentral();
    setupMenu();
    setupShortcuts();
    setupSignals();

    Settings::loadWindowSize(this);
    Settings::loadSplitterState(ui->splitter);

#ifdef INCLUDE_QBREEZE   // use QBreeze if it exists
    Settings::loadTheme(themesActionGroup);
#endif
#ifdef INSIDE_EDITOR
    if (Settings::containsUseEditor())
        editorWidgetAction->setChecked(Settings::loadUseEditor());
    else
        editorWidgetAction->setChecked(true);
    editorWidget->setVisible(editorWidgetAction->isChecked());
#endif
    emit started();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupCentral() {
    setMinimumSize(500, 300);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText(tr("Search"));
    searchLineEdit->setFixedWidth(300);
    eraseSearch = searchLineEdit->addAction(QIcon(":images/quit.png"), QLineEdit::TrailingPosition);
    eraseSearch->setVisible(false);
    auto w = new QWidget(this);
    new QHBoxLayout(w);
    w->layout()->addWidget(searchLineEdit);
    w->setContentsMargins(0, 0, 5, 0);
    ui->menubar->setCornerWidget(w);

    nbFiles = new QLabel(tr("no files"), ui->statusbar);
    ui->statusbar->addPermanentWidget(nbFiles);

    systray = new QSystemTrayIcon(QIcon(":images/icon128.png"), this);
    systray->show();

    spinnerLabel = new QLabel(this);
    auto* movie  = new QMovie(this);
    movie->setFileName(":images/spinner.gif");
    spinnerLabel->setMovie(movie);
    movie->setScaledSize(QSize(20, 20));
    movie->start();
    ui->statusbar->addPermanentWidget(spinnerLabel);
    spinnerLabel->setVisible(false);

#ifdef INSIDE_EDITOR
    editorWidget = new EditorWidget(this);
    ui->splitter->addWidget(editorWidget);
#endif
}

void MainWindow::setupMenu() {
    Settings::getActionsRecentlyOpenedFiles(ui->recentlyOpenedFilesMenu);

#ifdef INCLUDE_QBREEZE   // use QBreeze if it exists
    auto setStyleMenu = new QMenu(tr("Themes"), ui->menuEdit);
    themesActionGroup = new QActionGroup(ui->menuEdit);
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
    ui->menuEdit->addMenu(setStyleMenu);
#endif
#ifdef INSIDE_EDITOR
    editorWidgetAction = new QAction(tr("Show Integrated Reader"), ui->menuEdit);
    editorWidgetAction->setCheckable(true);
    editorWidgetAction->setChecked(false);
    ui->menuEdit->addAction(editorWidgetAction);
#endif

    ui->changeDataDirAction->setToolTip(Settings::dataDirectory());
    ui->openDataDirAction->setToolTip(Settings::dataDirectory());

    // systray menu
    auto systrayMenu  = new QMenu(this);
    systrayExitAction = new QAction(tr("Exit"), systrayMenu);
    systrayMenu->addAction(systrayExitAction);
    systray->setContextMenu(systrayMenu);
}

void MainWindow::setupShortcuts() {
    new QShortcut(QKeySequence("Ctrl+f"), searchLineEdit, SLOT(setFocus()));
    new QShortcut(QKeySequence("Ctrl+p"), ui->filesContainer, SLOT(pinSelected()));
    new QShortcut(QKeySequence("Ctrl+s"), ui->filesContainer, SLOT(starSelected()));
    new QShortcut(QKeySequence("Ctrl+r"), ui->filesContainer, SLOT(restoreSelected()));
    new QShortcut(QKeySequence(QKeySequence::Delete), ui->filesContainer, SLOT(trashSelected()));
    new QShortcut(QKeySequence(QKeySequence(Qt::Key_Shift + Qt::Key_Delete)), ui->filesContainer,
                  SLOT(permanentlyDeleteSelected()));
    auto edel = new QShortcut(QKeySequence("Ctrl+e"), this);
    connect(edel, &QShortcut::activated, this, [=] {
        if (ui->filesContainer->hasFocus())
            ui->filesContainer->editElement(ui->filesContainer->currentItem());
    });
    auto ret = new QShortcut(QKeySequence("Return"), this);
    connect(ret, &QShortcut::activated, this, [=] {
        if (ui->tagsContainer->hasFocus())
            ui->tagsContainer->selected();
        else if (ui->filesContainer->hasFocus())
            ui->filesContainer->openCurrent();
        else if (searchLineEdit->hasFocus())
            search();
    });
}

void MainWindow::setupSignals() {
    connect(ui->tagsContainer, &TagsContainer::itemSelected, ui->filesContainer,
            &FilesContainer::addFiles);
    connect(ui->tagsContainer, &TagsContainer::itemSelected, this,
            &MainWindow::changeNumberOfFilesLabel);
    connect(ui->filesContainer, &FilesContainer::numberOfElementsChanged, this,
            &MainWindow::changeNumberOfFilesLabel);
    connect(ui->filesContainer, &FilesContainer::elementChanged, ui->tagsContainer,
            &TagsContainer::reloadElement);
    connect(ui->splitter, &QSplitter::splitterMoved, this,
            [=]() { Settings::saveSplitterState(ui->splitter); });
    connect(ui->reloadElementsAction, &QAction::triggered, this, &MainWindow::reloadContent);
    connect(searchLineEdit, &QLineEdit::textEdited, this, &MainWindow::search);
    connect(eraseSearch, &QAction::triggered, [&] {
        if ((ui->tagsContainer->selectedItems().isEmpty()) && !(searchLineEdit->text().isEmpty()))
            ui->filesContainer->clearView();
        searchLineEdit->clear();
        eraseSearch->setVisible(false);
    });
    connect(searchLineEdit, &QLineEdit::textEdited, eraseSearch,
            [=] { eraseSearch->setVisible(!searchLineEdit->text().isEmpty()); });
    connect(ui->newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->recentlyOpenedFilesMenu, &QMenu::triggered, Settings::openFileAction);
    connect(ui->recentlyOpenedFilesMenu, &QMenu::triggered, this,
            [=]() { Settings::getActionsRecentlyOpenedFiles(ui->recentlyOpenedFilesMenu); });
    connect(ui->quitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->setMdReaderAction, &QAction::triggered, this,
            [=] { std::make_unique<ReadersDialog>(this); });
    connect(ui->changeDataDirAction, &QAction::triggered, this, &MainWindow::setDataDirectory);
    connect(ui->openDataDirAction, &QAction::triggered, this, [=] {
        if (Settings::dataDirectoryIsSet())
            QDesktopServices::openUrl(QUrl(Settings::dataDirectory()));
        else
            QMessageBox::warning(this, tr("No data directory"),
                                 tr("Data directory hasn't been set."));
    });
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(this, &MainWindow::started, this, &MainWindow::load,
            Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(ui->expandButton, &QPushButton::clicked, ui->tagsContainer,
            &TagsContainer::expandItems);
    connect(ui->collapseButton, &QPushButton::clicked, ui->tagsContainer,
            &TagsContainer::collapseItems);

    connect(ui->filesContainer, &FilesContainer::openedFile, this,
            [=]() { Settings::getActionsRecentlyOpenedFiles(ui->recentlyOpenedFilesMenu); });
    connect(ui->tagsContainer, &TagsContainer::loadingFiles, this,
            [=]() { spinnerLabel->setVisible(true); });
    connect(ui->tagsContainer, &TagsContainer::filesLoaded, spinnerLabel, &QLabel::hide);
    connect(ui->tagsContainer, &TagsContainer::loadingFiles, this,
            [=]() { disableSomeWidgets(true); });
    connect(ui->tagsContainer, &TagsContainer::filesLoaded, this,
            [=]() { disableSomeWidgets(false); });
    connect(ui->filesContainer, &FilesContainer::deletedItem, ui->tagsContainer,
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
    connect(ui->filesContainer, &FilesContainer::elementChanged, editorWidget,
            &EditorWidget::reload);
    connect(editorWidgetAction, &QAction::toggled, this, [=] {
        bool checked = editorWidgetAction->isChecked();
        editorWidget->setVisible(checked);
        Settings::saveUseEditor(checked);
        if (checked)
            connect(ui->filesContainer, &FilesContainer::selectionChanged_, editorWidget,
                    &EditorWidget::open);
        else {
            disconnect(ui->filesContainer, &FilesContainer::selectionChanged_, editorWidget,
                       &EditorWidget::open);
            editorWidget->closeFile();
        }
    });
#endif
}

void MainWindow::load() {
    qApp->processEvents();
    if (!Settings::dataDirectoryIsSet() || !QFile::exists(Settings::dataDirectory())) {
        setDataDirectory();
        if (!Settings::dataDirectoryIsSet()) return;
    }
    loadDataDirectoryContent();
    ui->tagsContainer->loadCollapseOrExpand();
}

void MainWindow::setDataDirectory() {
    auto dialog = new DataDirDialog(this);
    auto res    = dialog->exec();
    if (res == DataDirDialog::Accepted) reloadContent();
    delete dialog;
    QString dd = Settings::dataDirectory();
    ui->changeDataDirAction->setToolTip(dd);
    ui->openDataDirAction->setToolTip(dd);
}

void MainWindow::loadDataDirectoryContent() {
    const PathsList paths       = be::fetch_files(Settings::dataDirectory());
    const ElementsList elements = Element::constructElementList(paths);
    if (!elements.empty()) openElements(elements);
}

void MainWindow::reloadContent() {
#ifdef INSIDE_EDITOR
    editorWidget->clear();
#endif
    ui->tagsContainer->init();
    ui->filesContainer->clearView();
    loadDataDirectoryContent();
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
    ElementsList lst{ e };
    openElements(lst);
}

void MainWindow::search() {
    const QString line = searchLineEdit->text().simplified().toLower();
    ui->filesContainer->clearView();
    ui->tagsContainer->clearSelection();
    changeNumberOfFilesLabel();
    if (line.isEmpty()) return;

    QStringList keywords{ line.split(' ') };

    const auto* lst = TagsContainer::real(ui->tagsContainer->topLevelItem(0))->elements();
    auto res        = std::make_unique<QVector<Element*>>();

    auto contains = [&keywords](const QString& title) {
        for (const QString& s : keywords)
            if (!(title.contains(s))) return false;
        return true;
    };

    for (Element* e : *lst)
        if (contains(e->title().simplified().toLower())) res->push_back(e);

    ui->filesContainer->addFiles(res.get());
}

void MainWindow::openElements(const ElementsList& els) {
    ui->tagsContainer->addElements(els);
}

void MainWindow::disableSomeWidgets(const bool& disable) {
    ui->changeDataDirAction->setDisabled(disable);
    ui->reloadElementsAction->setDisabled(disable);
    searchLineEdit->setDisabled(disable);
}

void MainWindow::changeNumberOfFilesLabel() {
    int nb    = ui->filesContainer->count();
    QString s = QString::number(nb);
    QString res;
    if (nb == 0)
        res = tr("no files");
    else if (nb == 1)
        res = tr("1 file");
    else
        res = s + tr(" files");

    nbFiles->setText(res);
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
    Settings::saveUiSettings(size(), ui->splitter->saveState());
    event->accept();
}
