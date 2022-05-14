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
#include <QPlainTextEdit>

class MarkdownHighlighter;

class MarkdownEditorWidget : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit MarkdownEditorWidget(QWidget* parent = nullptr);
    ~MarkdownEditorWidget();
    void contextMenuEvent(QContextMenuEvent* event) override;
    inline void setOccupied(bool o) { m_occupied = o; }
    inline bool occupied() const { return m_occupied; }
    void clear();
    void setText(const QString& text);

signals:
    void toClose();

private:
    MarkdownHighlighter* highlighter;
    /* is there a file displayed or not */
    bool m_occupied;
};
