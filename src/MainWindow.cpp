#include "MainWindow.h"
#include <QAction>
#include <QDesktopServices>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QTextStream>
#include "./ui_MainWindow.h"
#include "Benchmark.h"
#include "DataDirectoryDialog.h"
#include "Document.h"
#include "DocumentInfoDialog.h"
#include "DocumentListModel.h"
#include "ExternalReadersDialog.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include "TagTreeItem.h"
#include "TagTreeModel.h"

using std::make_unique;
using Doc::Document;
namespace Settings = Ui::Settings;


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mTagTreeModel{ nullptr },
      mDocumentListModel{ nullptr }, mSearchBar{ nullptr }, mSearchBarEraseText{ nullptr },
      mNumberOfFilesLabel{ nullptr } {
    ui->setupUi(this);
    setup();
}


MainWindow::~MainWindow() {
    delete ui;

    clearDocuments();
}


void MainWindow::clearDocuments() {
    for (Document* doc : mDocuments) {
        delete doc;
    }

    mDocuments.clear();
}


void MainWindow::setup() {
    setupModels();
    setupMenuBar();
    setupSettings();
    setupSignalsAndSlots();
    setupMenu();
    setupKeyboardShortcuts();
    setupSystemTray();

    emit loadedUi();
}


void MainWindow::setupModels() {
    mTagTreeModel      = make_unique<Ui::Models::TagTreeModel>();
    mDocumentListModel = make_unique<Ui::Models::DocumentListModel>();
    ui->tagsTreeView->setModel(mTagTreeModel.get());
    ui->documentsListView->setModel(mDocumentListModel.get());
}


void MainWindow::setupMenuBar() {
    mSearchBar = make_unique<QLineEdit>();
    mSearchBar->setFixedWidth(300);
    mSearchBarEraseText = std::unique_ptr<QAction>(mSearchBar->addAction(
        QIcon(QStringLiteral(":images/quit.png")), QLineEdit::TrailingPosition));

    mSearchBarEraseText->setVisible(false);
    auto cornerWidget = new QWidget(this);
    (new QHBoxLayout(cornerWidget))->addWidget(mSearchBar.get());
    ui->menubar->setCornerWidget(cornerWidget);

    // label in the status bar
    mNumberOfFilesLabel = make_unique<QLabel>("", ui->statusbar);
    ui->statusbar->addPermanentWidget(mNumberOfFilesLabel.get());
}


void MainWindow::setupSettings() {
    // Size of the window
    auto sz = Settings::loadWindowSize();
    if (sz.isValid()) {
        resize(sz);
    }

    if (auto splitterData = Settings::loadSplitterState(); !splitterData.isEmpty()) {
        ui->splitter->restoreState(splitterData);
    }

    // Document Viewer visible by default
    if (!Settings::hasLoadDocumentViewerVisibility()) {
        Settings::saveDocumentViewerVisibility(true);
    }

    bool visibleDocumentViewer = Settings::loadDocumentViewerVisibility();
    Settings::saveDocumentViewerVisibility(visibleDocumentViewer);
    ui->documentContentView->setVisible(visibleDocumentViewer);
    ui->showDocumentContentViewAction->setChecked(visibleDocumentViewer);
}


void MainWindow::setupSignalsAndSlots() {
    using namespace Ui::Models;
    connect(this, &MainWindow::loadedUi, this, &MainWindow::loadDocuments,
            Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(ui->tagsTreeView, &TagsTreeView::tagsSelected, this, &MainWindow::onTagClicked);
    connect(ui->documentsListView, &DocumentsListView::documentSelected, ui->documentContentView,
            &DocumentContentView::setDocument);
    connect(mDocumentListModel.get(), &DocumentListModel::documentDeleted, ui->documentContentView,
            &DocumentContentView::onDocumentDeleted);
    connect(mDocumentListModel.get(), &DocumentListModel::documentPermanentlyDeleted, this,
            &MainWindow::onDocumentPermanentlyDeleted);
    connect(mDocumentListModel.get(), &DocumentListModel::tagAdded, mTagTreeModel.get(),
            &TagTreeModel::addTag);
    connect(mDocumentListModel.get(), &DocumentListModel::tagDeleted, mTagTreeModel.get(),
            &TagTreeModel::deleteTag);
    connect(ui->documentsListView, &DocumentsListView::documentToOpen, this, &MainWindow::openFile);
    connect(mSearchBar.get(), &QLineEdit::textChanged, this, &MainWindow::search);
    connect(mSearchBar.get(), &QLineEdit::textChanged, mSearchBarEraseText.get(),
            [this] { mSearchBarEraseText->setVisible(!mSearchBar->text().isEmpty()); });
    connect(mDocumentListModel.get(), &DocumentListModel::numberOfDocumentsChanged, this,
            &MainWindow::onNumberOfDocumentsChanged);
    connect(mSearchBarEraseText.get(), &QAction::triggered, mSearchBar.get(), &QLineEdit::clear);
    connect(ui->expandButton, &QPushButton::clicked, ui->tagsTreeView, &TagsTreeView::expandAll);
    connect(ui->collapseButton, &QPushButton::clicked, ui->tagsTreeView,
            &TagsTreeView::collapseAll);
}


void MainWindow::setupMenu() {
    // File
    connect(ui->newFileAction, &QAction::triggered, this, &MainWindow::createNewFile);
    connect(ui->changeDataDirAction, &QAction::triggered, this, &MainWindow::onChangeDataDirAction);
    connect(ui->openDataDirAction, &QAction::triggered, this,
            [] { QDesktopServices::openUrl(QUrl::fromLocalFile(Settings::loadDataDirectory())); });
    connect(ui->quitAction, &QAction::triggered, this, &QApplication::exit);
    connect(ui->settingsAction, &QAction::triggered, this, &MainWindow::settingsDialog);

    // recent files
    QMenu* recFiles = ui->recentlyOpenedFilesMenu;
    for (const QString& file : Ui::Settings::loadRecentFiles()) {
        recFiles->addAction(file, this, [=] { openFile(file); });
    }

    // QBreeze themes
    auto themesActionGroup = new QActionGroup(ui->menuEdit);
    themesActionGroup->setExclusive(true);
    ui->nativeThemeAction->setData(QStringLiteral("native"));
    ui->qbreezeDarkAction->setData(QStringLiteral(":qbreeze/dark.qss"));
    ui->qbreezeLightAction->setData(QStringLiteral(":qbreeze/light.qss"));

    const QString currentTheme = Settings::loadTheme();
    applyTheme(currentTheme);

    for (auto act : { ui->nativeThemeAction, ui->qbreezeDarkAction, ui->qbreezeLightAction }) {
        themesActionGroup->addAction(act);
        if (act->data().toString() == currentTheme) {
            act->setChecked(true);
        }
    }

    if (currentTheme == "") {
        ui->nativeThemeAction->setChecked(true);
    }

    connect(themesActionGroup, &QActionGroup::triggered, this, [this](QAction* act) {
        applyTheme(act->data().toString());
        Settings::saveTheme(act->data().toString());
        act->setChecked(true);
    });


    // Edit
    connect(ui->showDocumentContentViewAction, &QAction::triggered, this,
            &MainWindow::toggleDocumentContentViewVisibility);
    connect(ui->markdownReadersAction, &QAction::triggered, this,
            [&] { make_unique<ExternalReadersDialog>(this); });
    connect(ui->reloadElementsAction, &QAction::triggered, this, &MainWindow::loadDocuments);

    // Help
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::about);
}


void MainWindow::setupKeyboardShortcuts() {
    const auto ctrl_f  = QKeySequence::fromString(QStringLiteral("Ctrl+f"));
    const auto return_ = QKeySequence::fromString(QStringLiteral("Return"));

    new QShortcut(ctrl_f, this, [this] { mSearchBar->setFocus(); });
    new QShortcut(return_, this, [this] { on_ReturnPressed(); });
}


void MainWindow::on_ReturnPressed() {
    if (ui->documentsListView->hasFocus()) {
        ui->documentsListView->openCurrentDocument();
    } else if (ui->tagsTreeView->hasFocus()) {
        ui->tagsTreeView->onClicked({});
    } else if (mSearchBar->hasFocus()) {
        search(mSearchBar->text());
    }
}


void MainWindow::setupSystemTray() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    mSystemTrayIcon = std::make_unique<QSystemTrayIcon>(QIcon(":images/icon128.png"), this);
    mSystemTrayIcon->show();

    mSystemTrayMenu = std::make_unique<QMenu>(this);
    mSystemTrayMenu->addAction(tr("Show DeepTags"), this, [=] { setVisible(true); });
    mSystemTrayMenu->addSeparator();
    mSystemTrayMenu->addAction(tr("Exit"), this, SLOT(close()));
    mSystemTrayIcon->setContextMenu(mSystemTrayMenu.get());

    connect(mSystemTrayIcon.get(), &QSystemTrayIcon::activated, this,
            &MainWindow::onSystemTryIconActivated);
}


void MainWindow::onSystemTryIconActivated(int reason) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    switch (reason) {
    case QSystemTrayIcon::Unknown:
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::MiddleClick:
        break;
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
        setVisible(!isVisible());
        break;
    }
}


void MainWindow::loadDocuments() {
    qApp->processEvents();
    mTagTreeModel->reset();
    mDocumentListModel->reset();
    ui->documentContentView->reset();
    clearDocuments();

    QString dataDirectory = Ui::Settings::loadDataDirectory();

    if (dataDirectory.isEmpty() || !QDir(dataDirectory).exists()) {
        onChangeDataDirAction();
        return;
    }

    auto pathList = Doc::Utils::getPathList(dataDirectory);
    mDocuments    = Document::constructDocumentList(pathList);

    mTagTreeModel->setDocuments(&mDocuments);
}


QVector<Document*> filterDocuments(const QString& tag, const QVector<Document*>& source) {
    QVector<Document*> docs;
    if (tag == "All Notes") {
        for (auto doc : source) {
            if (!doc->isDeleted()) {
                docs.push_back(doc);
            }
        }
    } else if (tag == "Trash") {
        for (auto doc : source) {
            if (doc->isDeleted()) {
                docs.push_back(doc);
            }
        }
    } else if (tag == "Favorite") {
        for (auto doc : source) {
            if (doc->isFavorited() && !doc->isDeleted()) {
                docs.push_back(doc);
            }
        }
    } else if (tag == "Untagged") {
        for (auto doc : source) {
            if (doc->getTags().empty() && !doc->isDeleted()) {
                docs.push_back(doc);
            }
        }
    } else {
        const QStringVector tagChain = Doc::Utils::deconstructTag(tag);
        for (auto doc : source) {
            if (doc->containsTag(tagChain) && !doc->isDeleted()) {
                docs.push_back(doc);
            }
        }
    }

    return docs;
}


void MainWindow::onTagClicked(const QStringList& tags) {
    Benchmark b("Document filtering");
    if (tags.isEmpty()) {
        return;
    }

    auto docs = filterDocuments(tags.at(0), mDocuments);
    for (int i = 1; i < tags.size(); i++) {
        docs = filterDocuments(tags.at(i), docs);
    }

    auto isNotSpecial = [&](const QString& t) {
        return !Ui::Models::TagTreeItem::specialTags.contains(t);
    };
    QStringList nonSpecialTags;
    std::copy_if(tags.begin(), tags.end(), std::back_inserter(nonSpecialTags), isNotSpecial);

    mDocumentListModel->setDocuments(docs, nonSpecialTags);
}


void MainWindow::toggleDocumentContentViewVisibility() {
    bool visible = !Settings::loadDocumentViewerVisibility();
    ui->documentContentView->setVisible(visible);
    ui->showDocumentContentViewAction->setChecked(visible);
    Settings::saveDocumentViewerVisibility(visible);
}


void MainWindow::onChangeDataDirAction() {
    auto dialog = make_unique<DataDirectoryDialog>(this);
    connect(dialog.get(), &DataDirectoryDialog::dataDirectoryChanged, this,
            &MainWindow::loadDocuments);

    dialog->exec();

    disconnect(dialog.get(), &DataDirectoryDialog::dataDirectoryChanged, this,
               &MainWindow::loadDocuments);
}


void MainWindow::openFile(QString path, QString editor) {
    if (path.isEmpty() || !QFile::exists(path)) {
        QMessageBox::critical(this, tr("Error"), tr("This file doesn't exist"));
        return;
    }

    if (editor.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        editor = editor.simplified();
        if (editor.indexOf(' ') == -1) {
            QProcess::startDetached(editor, { path });
        } else {
            QStringList args = editor.split(" ");
            const QString ed = args.at(0);
            args.removeAt(0);
            args.append(path);
            QProcess::startDetached(ed, args);
        }
    }

    Ui::Settings::addRecentFile(path);
}


void MainWindow::createNewFile() {
    auto dialog = make_unique<DocumentInfoDialog>(nullptr, this);
    if (dialog->exec() == dialog->Rejected) {
        return;
    }

    QString dd    = Settings::loadDataDirectory();
    QString title = dialog->getTitle();
    auto filePath = Doc::Utils::createNewFile(dd, title);
    if (!filePath.has_value()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to create a new file"));
        return;
    }

    auto newDoc = new Document(filePath.value());
    if (dialog->isFavorited()) {
        Doc::Utils::setDocumentFavorited(newDoc, true);
    }

    if (dialog->isPinned()) {
        Doc::Utils::setDocumentPinned(newDoc, true);
    }

    if (auto tags = dialog->getTags(); !tags.isEmpty()) {
        for (const QString& tag : tags) {
            Doc::Utils::addDocumentTag(newDoc, tag);
        }
    }

    for (const QString& tag : newDoc->getTags()) {
        mTagTreeModel->addTag(tag);
    }

    mDocuments.push_back(newDoc);

    if (newDoc->containsAllTags(mDocumentListModel->getActiveTag())) {
        mDocumentListModel->addDocument(newDoc);
    }
}


void MainWindow::settingsDialog() {
    const auto oldDataDir         = Settings::loadDataDirectory();
    const auto oldExternalReaders = Settings::loadExternalReaders();
    auto dialog                   = make_unique<SettingsDialog>(this);

    if (dialog->exec() == dialog->Rejected) {
        return;
    }

    if (auto newDataDir = Settings::loadDataDirectory(); newDataDir != oldDataDir) {
        loadDocuments();
    }
}


void MainWindow::search(const QString& text) {
    Benchmark b("searching");
    if (text.isEmpty()) {
        mDocumentListModel->setDocuments({}, {});
        return;
    }

    QVector<Document*> documents;
    const auto keywords = Doc::Utils::split(text.simplified().toLower(), " ");

    const auto contains = [&keywords](const QString& title) {
        for (const QString& s : keywords) {
            if (!title.contains(s)) {
                return false;
            }
        }

        return true;
    };

    for (auto doc : mDocuments) {
        if (contains(doc->getTitle().simplified().toLower()) && !doc->isDeleted()) {
            documents.push_back(doc);
        }
    }

    mDocumentListModel->setDocuments(documents, {});
}


void MainWindow::onNumberOfDocumentsChanged(int number) {
    const QString number_ = QString::number(number);
    QString result;

    if (number == 0) {
        result = "";
    } else if (number == 1) {
        result = number_ + tr((" document"));
    } else {
        result = number_ + tr(" documents");
    }

    mNumberOfFilesLabel->setText(result);
}


void MainWindow::onDocumentPermanentlyDeleted(Document* document) {
    const QString path = document->getPath();
    for (int i = 0; i < mDocuments.size(); i++) {
        if (mDocuments.at(i)->getPath() == path) {
            delete mDocuments.takeAt(i);
        }
    }
}


void MainWindow::applyTheme(const QString& theme) {
    if (theme == "native" || theme == "") {
        qApp->setStyleSheet("");
        return;
    }

    QFile f(theme);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());
}


void MainWindow::about() {
    auto str = QStringLiteral("\
        <br><b>Name: </b>DeepTags<br>\
        <b>Version: %1</b>  (compiled in %2 at %3)\
        <br><b>Author:</b> Zineddine SAIBI.<br>\
        <b>E-mail: </b> saibi.zineddine@yahoo.com<br>\
        <b>website: </b> https://github.com/SZinedine/DeepTags <br>")
                   .simplified()
                   .arg(qApp->applicationVersion(), __DATE__, __TIME__);

    QMessageBox::about(this, tr("About"), str);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    Settings::saveUiSettings(size(), ui->splitter->saveState());
    event->accept();
}
