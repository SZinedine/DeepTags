/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2021  Zineddine Saibi
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
#include "markdowneditorwidget.h"


class QFileSystemWatcher;
// class QToolBar;

class EditorWidget : public QWidget {
    Q_OBJECT
public:
    EditorWidget(QWidget* parent = nullptr);
    ~EditorWidget();

    void open(QString path);
    void display(QString path);
    void closeFile();
    void reload();
    void clear();

signals:
    void openedFile(QString file);
    void savedFile(QString file);
    void closedFile(QString file);

private:
    MarkdownEditorWidget* m_editor;
    QFileSystemWatcher* m_watcher;
    // QToolBar* m_toolBar;
    QString m_currentPath;
    QString m_fileContent;
};
