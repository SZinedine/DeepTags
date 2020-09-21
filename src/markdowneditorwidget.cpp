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
#include "markdowneditorwidget.h"
#include <QContextMenuEvent>
#include <QMenu>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget* parent)
    : QMarkdownTextEdit(parent), m_occupied(false) {
    setReadOnly(true);
}

void MarkdownEditorWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu              = createStandardContextMenu();
    QAction* closeFileAction = new QAction(tr("Close File"), this);
    menu->addSeparator();
    menu->addAction(closeFileAction);
    closeFileAction->setEnabled(m_occupied);

    connect(closeFileAction, &QAction::triggered, [this] { emit this->toClose(); });
    menu->exec(event->globalPos());

    delete menu;
    delete closeFileAction;
}

void MarkdownEditorWidget::clear() {
    QPlainTextEdit::clear();
    setOccupied(false);
}

void MarkdownEditorWidget::setText(const QString& text) {
    QMarkdownTextEdit::setText(text);
    setOccupied(true);
}
