#include "settings.h"

#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <thread>

#include "../element/element.h"
#include "readersdialog.h"


void Settings::saveUiSettings(const QSize& windowSize, const QByteArray& splitterState) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("window_size", QVariant(windowSize));    // size of the window
    s.setValue("splitter_size", splitterState);
    s.endGroup();
}

void Settings::loadSplitterState(QSplitter* splitter) {
    QSettings s;
    s.beginGroup("main");
    splitter->restoreState(s.value("splitter_size").toByteArray());
    s.endGroup();
}

void Settings::saveSplitterState(QSplitter* splitter) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("splitter_size", splitter->saveState());
    s.endGroup();
}

void Settings::loadWindowSize(MainWindow* w) {
    QSettings s;
    s.beginGroup("main");
    w->resize(s.value("window_size").toSize());
    s.endGroup();
}


void Settings::saveEditors(const QStringList& lst) {
    QSettings s;
    s.beginGroup("markdown_editors");
    s.setValue("list", lst);
    s.endGroup();
    if (!lst.isEmpty())
        saveMainEditor(lst.at(0));
    else
        saveMainEditor("");
}

QStringList Settings::mdEditors() {
    QSettings s;
    s.beginGroup("markdown_editors");
    QStringList lst(s.value("list").toStringList());
    s.endGroup();

    return lst;
}

void Settings::saveMainEditor(const QString& editor) {
    QSettings s;
    s.beginGroup("markdown_editors");
    s.setValue("main", editor);
    s.endGroup();
}

QString Settings::mainMdEditor() {
    QSettings s;
    s.beginGroup("markdown_editors");
    QString e = s.value("main").toString();
    s.endGroup();

    return e;
}


bool Settings::setDataDirectory() {
    QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Open Directory"), dataDirectory());
    if (dir.isEmpty()) return false;

    QSettings s;
    s.beginGroup("main");
    s.setValue("data_dir", dir);
    if (!s.contains("last_dir")) s.setValue("last_dir", dir);
    s.endGroup();
    return true;
}

QString Settings::dataDirectory() {
    if (!dataDirectoryIsSet()) return QDir::homePath();
    QSettings s;
    s.beginGroup("main");
    QString dir = s.value("data_dir").toString();
    s.endGroup();

    return dir;
}

bool Settings::dataDirectoryIsSet() {
    QSettings s;
    s.beginGroup("main");
    bool res = s.contains("data_dir");
    s.endGroup();
    return res;
}


void Settings::expand(const bool& expanded) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("expanded", expanded);
    s.endGroup();
}


bool Settings::expandedItems() {
    QSettings s;
    s.beginGroup("main");
    bool res = s.value("expanded", true).toBool();
    s.endGroup();
    return res;
}


void Settings::saveRecentlyOpenedFile(const fs::path& p) {
    QStringList paths = getRawRecentlyOpenedFiles();
    // remove the entry from the list if it already exists
    QString string = QString(p.string().c_str());
    while (paths.contains(string)) paths.removeAt(paths.indexOf(string));

    paths.prepend(string);
    saveRecentlyOpenedFiles(paths);
}

void Settings::saveRecentlyOpenedFiles(QStringList& paths) {
    // only save the first 15 files
    const int maxSize = 15;
    while (paths.length() > maxSize) paths.removeLast();

    QSettings s;
    s.beginGroup("files");
    s.setValue("recently_opened_files", paths);
    s.endGroup();
}

QMenu* Settings::getActionsRecentlyOpenedFiles(QMenu* menu) {
    if (!menu) return nullptr;
    menu->clear();
    QStringList raw = getRawRecentlyOpenedFiles();
    if (raw.isEmpty()) return nullptr;

    for (const QString& path : raw) {    // qaction data = path
        if (!fs::exists(fs::path(path.toStdString().c_str()))) continue;
        Element e(fs::path(path.toStdString()));    // make it more efficient by using low level api
        QString title(e.title().c_str());
        auto    action = new QAction(title, menu);
        action->setToolTip(path);
        action->setData(QVariant(path));
        menu->addAction(action);
    }
    menu->addSeparator();

    auto clearAct = new QAction(tr("Clear"), menu);
    connect(clearAct, &QAction::triggered, [=] {
        menu->clear();
        eraseRecentlyOpenedFiles();
    });
    menu->addAction(clearAct);

    menu->setToolTipsVisible(true);
    return menu;
}

void Settings::eraseRecentlyOpenedFiles() {
    QStringList s;
    saveRecentlyOpenedFiles(s);
}

QStringList Settings::getRawRecentlyOpenedFiles() {
    QSettings s;
    s.beginGroup("files");
    QStringList sl = s.value("recently_opened_files").toStringList();
    s.endGroup();
    return sl;
}


void Settings::openFileAction(QAction* action) {
    QVariant data = action->data();
    if (data.isNull()) return;    // in case clear is triggered
    fs::path p(action->data().toString().toStdString().c_str());
    openFile("", p, action->parentWidget());
}


void Settings::openFile(QString editor, const fs::path& path, QWidget* parent) {
    if (!fs::exists(path)) {
        QMessageBox::critical(parent, tr("Error"), tr("This file doesn't exist"));
        return;
    }
    if (editor.isEmpty()) editor = mainMdEditor();
    if (editor.isEmpty()) {
        QMessageBox::critical(parent, "Set a Markdown editor", "No Markdown editor set");
        return;
    }
    QString command = editor + QString(" \"") + QString(path.string().c_str()) + QString("\"");
    std::thread([=] { std::system(command.toStdString().c_str()); }).detach();
    saveRecentlyOpenedFile(path);
}

void Settings::saveTheme(QAction* ac) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("theme", ac->data());
    s.endGroup();
    ac->setChecked(true);
    applyTheme(ac->data().toString());
}

void Settings::loadTheme(QActionGroup* ag) {
    QSettings s;
    s.beginGroup("main");
    QString theme = s.value("theme").toString();
    s.endGroup();
    applyTheme(theme);
    for (QAction* ac : ag->actions())
        if (ac->data().toString() == theme) ac->setChecked(true);
}


void Settings::applyTheme(const QString& theme) {
    if (theme == "native" || theme == "") {
        qApp->setStyleSheet("");
        return;
    }
    QFile f(theme);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());
}


void Settings::setTagItemColor(const QString& item, const QString& color) {
    auto map  = getTagItemColor();
    map[item] = QVariant(color);
    if (color.isEmpty()) map.remove(item);
    QSettings s;
    s.beginGroup("main");
    s.setValue("item_color", map);
    s.endGroup();
}

QHash<QString, QVariant> Settings::getTagItemColor() {
    QHash<QString, QVariant> map;
    QSettings               s;
    s.beginGroup("main");
    map = s.value("item_color").toHash();
    s.endGroup();
    return map;
}
