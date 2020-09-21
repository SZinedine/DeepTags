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
#include "editorwidget.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <iostream>

EditorWidget::EditorWidget(QWidget* parent)
    : QWidget(parent), m_currentPath(""), m_fileContent("") {
    auto layout = new QVBoxLayout;
    setLayout(layout);
    // m_toolBar = new QToolBar(this);
    m_editor = new MarkdownEditorWidget(this);
    // layout->addWidget(m_toolBar);
    layout->addWidget(m_editor);
    layout->setContentsMargins(0, 0, 0, 0);
    // m_toolBar->addAction("save", [&] { save(); });
    m_editor->setReadOnly(true);
    m_watcher = new QFileSystemWatcher(this);

    // connect(this, &EditorWidget::openedFile, this, [=] { setVisible(true); });
    // connect(this, &EditorWidget::closedFile, this, [=] { setVisible(false); });
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, [=] { reload(); });
}


void EditorWidget::open(QString path) {
    if (path == m_currentPath) {
        reload();
        return;
    }
    closeFile();
    if (path.isEmpty()) return;
    m_currentPath = path;
    display(path);
    emit openedFile(path);
    setVisible(true);
    m_watcher->addPath(path);
}


void EditorWidget::display(QString path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream strm(&f);
    strm.setCodec("UTF-8");
    m_fileContent = strm.readAll();
    m_editor->setText(m_fileContent);
}

void EditorWidget::closeFile() {
    if (m_currentPath.isEmpty() || m_fileContent.isEmpty()) return;
    m_watcher->removePath(m_currentPath);
    m_editor->clear();
    m_currentPath.clear();
    m_fileContent.clear();
    emit closedFile(m_currentPath);
}

void EditorWidget::reload() {
    QTextCursor cur(m_editor->textCursor());
    auto po = cur.position();
    display(m_currentPath);
    cur.setPosition(po);
    m_editor->setTextCursor(cur);
}
