/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2022  Zineddine Saibi
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
#include "settings.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QVariant>
#include <thread>
#include "element.h"
#include "mainwindow.h"


void Settings::saveValue(const QString& group, const QString& label, const QVariant& value) {
    QSettings s;
    s.beginGroup(group);
    s.setValue(label, value);
    s.endGroup();
}

QVariant Settings::getValue(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    QVariant val = s.value(label);
    s.endGroup();
    return val;
}

bool Settings::contains(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    const bool val = s.contains(label);
    s.endGroup();
    return val;
}

void Settings::saveUiSettings(const QSize& windowSize, const QByteArray& splitterState) {
    saveValue("main", "window_size", QVariant(windowSize));   // size of the window
    saveValue("main", "splitter_size", QVariant(splitterState));
}

void Settings::loadSplitterState(QSplitter* splitter) {
    const auto val = getValue("main", "splitter_size").toByteArray();
    splitter->restoreState(val);
}

void Settings::saveSplitterState(QSplitter* splitter) {
    saveValue("main", "splitter_size", splitter->saveState());
}

void Settings::loadWindowSize(MainWindow* w) {
    const auto val = getValue("main", "window_size").toSize();
    w->resize(val);
}

void Settings::saveEditors(const QStringList& lst) {
    saveValue("markdown_editors", "list", lst);
    if (!lst.isEmpty())
        saveMainEditor(lst.at(0));
    else
        saveMainEditor("");
}

QStringList Settings::mdEditors() {
    return getValue("markdown_editors", "list").toStringList();
}

void Settings::saveMainEditor(const QString& editor) {
    saveValue("markdown_editors", "main", editor);
}

QString Settings::mainMdEditor() {
    return getValue("markdown_editors", "main").toString();
}

void Settings::setDataDirectory(QString dataDirectory) {
    if (dataDirectory.isEmpty()) return;
    saveValue("main", "data_dir", dataDirectory);
}

QString Settings::dataDirectory() {
    if (!dataDirectoryIsSet()) return QDir::homePath();
    return getValue("main", "data_dir").toString();
}

bool Settings::dataDirectoryIsSet() {
    return contains("main", "data_dir");
}

void Settings::expand(const bool& expanded) {
    saveValue("main", "expanded", expanded);
}

bool Settings::expandedItems() {
    return getValue("main", "expanded").toBool();
}

void Settings::saveRecentlyOpenedFile(const QString& p) {
    QStringList paths = getRawRecentlyOpenedFiles();
    // remove the entry from the list if it already exists
    while (paths.contains(p)) paths.removeAt(paths.indexOf(p));

    paths.prepend(p);
    saveRecentlyOpenedFiles(paths);
}

void Settings::saveRecentlyOpenedFiles(QStringList& paths) {
    const int maxSize = 15;   // only save the first 15 files
    while (paths.length() > maxSize) paths.removeLast();
    saveValue("files", "recently_opened_files", paths);
}

QMenu* Settings::getActionsRecentlyOpenedFiles(QMenu* menu) {
    if (!menu) return nullptr;
    menu->clear();
    QStringList raw = getRawRecentlyOpenedFiles();
    if (raw.isEmpty()) return nullptr;

    for (auto path : raw) {   // qaction data = path
        if (!QFile::exists(path)) continue;
        Element e(path);
        auto action = new QAction(e.title(), menu);
        action->setToolTip(path);
        action->setData(QVariant(path));
        menu->addAction(action);
    }
    menu->addSeparator();

    auto clearAct = new QAction(QObject::tr("Clear"), menu);
    QObject::connect(clearAct, &QAction::triggered, [=] {
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
    return getValue("files", "recently_opened_files").toStringList();
}

void Settings::openFileAction(QAction* action) {
    QVariant data = action->data();
    if (data.isNull()) return;   // in case clear is triggered
    QString p(action->data().toString());
    openFile("", p, action->parentWidget());
}

void Settings::openFile(QString editor, const QString& path, QWidget* parent) {
    if (!QFile::exists(path)) {
        QMessageBox::critical(parent, QObject::tr("Error"), QObject::tr("This file doesn't exist"));
        return;
    }
    if (editor.isEmpty()) editor = mainMdEditor();
    if (editor.isEmpty()) {
        QFileInfo p(path);
        QDir::setCurrent(p.absolutePath());
        QDesktopServices::openUrl(QUrl::fromLocalFile(p.fileName()));
    } else {
        QString p       = path;
        p               = p.replace("\"", "\\\"");
        QString command = editor + " \"" + p + "\"";
        std::thread([=] { std::system(command.toStdString().c_str()); }).detach();
    }
    saveRecentlyOpenedFile(path);
}

void Settings::saveTheme(QAction* ac) {
    saveValue("main", "theme", ac->data());
    ac->setChecked(true);
    applyTheme(ac->data().toString());
}

void Settings::loadTheme(QActionGroup* ag) {
    QString theme = getValue("main", "theme").toString();
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
    saveValue("main", "item_color", map);
}

QHash<QString, QVariant> Settings::getTagItemColor() {
    return getValue("main", "item_color").toHash();
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
    saveValue("main", "item_pinned", lst);
}

void Settings::setTagUnpinned(const QString& item) {
    auto lst = getTagPinned();
    if (!lst.contains(item)) return;
    lst.removeAll(item);
    saveValue("main", "item_pinned", lst);
}

QStringList Settings::getTagPinned() {
    return getValue("main", "item_pinned").toStringList();
}

void Settings::saveUseEditor(bool use) {
    saveValue("main", "use_integrated_editor", QVariant(use));
}

bool Settings::loadUseEditor() {
    return getValue("main", "use_integrated_editor").toBool();
}

bool Settings::containsUseEditor() {
    return contains("main", "use_integrated_editor");
}

void Settings::saveLineBreak(const QString& lb) {
    const QString x = (lb == "lf") ? "lf" : "crlf";
    saveValue("main", "line_break", x);
    loadLineBreak();
}

const QString Settings::getSavedLineBreakName() {
    const QString s(getValue("main", "line_break").toString().toLower());
    return (s == "lf" || s == "crlf") ? s : "crlf";
}

QString Settings::getSavedLineBreak() {
    return (getSavedLineBreakName() == "lf") ? "\n" : "\r\n";
}

extern QString LineBreak;

void Settings::loadLineBreak() {
    LineBreak = getSavedLineBreak();
}
