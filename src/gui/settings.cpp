#include "settings.h"
#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QStringList>
#include <QVariant>
#include <QWidget>
#include <thread>
#include "../element/element.h"
#include "readersdialog.h"

void Settings::saveString(const QString& group, const QString& label, const QString& value) {
    QSettings s;
    s.beginGroup(group);
    s.setValue(label, QVariant(value));
    s.endGroup();
}

QString Settings::getString(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    QString val = s.value(label).toString();
    s.endGroup();
    return val;
}

void Settings::saveStringList(const QString& group, const QString& label,
                              const QStringList& value) {
    QSettings s;
    s.beginGroup(group);
    s.setValue(label, QVariant(value));
    s.endGroup();
}

QStringList Settings::getStringList(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    QStringList val = s.value(label).toStringList();
    s.endGroup();
    return val;
}


void Settings::saveUiSettings(const QSize& windowSize, const QByteArray& splitterState) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("window_size", QVariant(windowSize));   // size of the window
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
    saveStringList("markdown_editors", "list", lst);
    if (!lst.isEmpty())
        saveMainEditor(lst.at(0));
    else
        saveMainEditor("");
}

QStringList Settings::mdEditors() {
    return getStringList("markdown_editors", "list");
}

void Settings::saveMainEditor(const QString& editor) {
    saveString("markdown_editors", "main", editor);
}

QString Settings::mainMdEditor() {
    return getString("markdown_editors", "main");
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
    return getString("main", "data_dir");
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
    bool res = s.value("expanded").toBool();
    s.endGroup();
    return res;
}


void Settings::saveRecentlyOpenedFile(const QString& p) {
    QStringList paths = getRawRecentlyOpenedFiles();
    // remove the entry from the list if it already exists
    while (paths.contains(p)) paths.removeAt(paths.indexOf(p));

    paths.prepend(p);
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

    for (const QString& path : raw) {   // qaction data = path
        if (QFile::exists(path)) continue;
        Element e(path);
        auto action = new QAction(e.title(), menu);
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
    return getStringList("files", "recently_opened_files");
}


void Settings::openFileAction(QAction* action) {
    QVariant data = action->data();
    if (data.isNull()) return;   // in case clear is triggered
    QString p(action->data().toString());
    openFile("", p, action->parentWidget());
}


void Settings::openFile(QString editor, const QString& path, QWidget* parent) {
    if (!QFile::exists(path)) {
        QMessageBox::critical(parent, tr("Error"), tr("This file doesn't exist"));
        return;
    }
    if (editor.isEmpty()) editor = mainMdEditor();
    if (editor.isEmpty()) {
        QFileInfo p(path);
        QDir::setCurrent(p.absolutePath());
        QDesktopServices::openUrl(QUrl::fromLocalFile(p.fileName()));
    }
    else {
        QString command = editor + " \"" + path + "\"";
        std::thread([=] { std::system(command.toStdString().c_str()); }).detach();
    }
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
    QString theme = getString("main", "theme");
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
    QSettings s;
    s.beginGroup("main");
    map = s.value("item_color").toHash();
    s.endGroup();
    return map;
}

void Settings::clearColorItems() {
    QSettings s;
    s.beginGroup("main");
    s.remove("item_color");
    s.endGroup();
}


void Settings::setTagPinned(const QString& item) {
    auto lst = getTagPinned();
    if (lst.contains(item)) return;
    lst.append(item);
    saveStringList("main", "item_pinned", lst);
}

void Settings::setTagUnpinned(const QString& item) {
    auto lst = getTagPinned();
    if (!lst.contains(item)) return;
    lst.removeAll(item);
    saveStringList("main", "item_pinned", lst);
}

QStringList Settings::getTagPinned() {
    return getStringList("main", "item_pinned");
}

void Settings::clearPinnedItems() {
    saveStringList("main", "item_pinned", {});
}

void Settings::saveUseEditor(bool use) {
    QSettings s;
    s.beginGroup("main");
    s.setValue("use_integrated_editor", QVariant(use));
    s.endGroup();
}

bool Settings::loadUseEditor() {
    QSettings s;
    s.beginGroup("main");
    bool res = s.value("use_integrated_editor").toBool();
    s.endGroup();
    return res;
}

bool Settings::containsUseEditor() {
    QSettings s;
    s.beginGroup("main");
    bool res = s.contains("use_integrated_editor");
    s.endGroup();
    return res;
}
