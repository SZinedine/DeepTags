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
#ifndef FILEITEM_H
#define FILEITEM_H

#include <QListWidgetItem>
#include "element.h"

class FilesContainer;

class FileItem : public QListWidgetItem {
public:
    FileItem(FilesContainer* parent = nullptr);
    FileItem(Element* element, FilesContainer* parent = nullptr);
    FileItem(const FileItem& other);
    FileItem(FileItem&& other);
    FileItem& operator=(const FileItem& other);
    FileItem& operator=(FileItem& other);
    FileItem& operator=(FileItem&& other);

    void setElement(Element* element);
    inline void setLabel(const QString& label) { setText(label); }
    inline QString path() const { return m_element->path(); }
    inline QString pathQstr() const { return m_element->path(); }
    inline QString title() const { return m_element->title(); }
    inline Tags tags() const { return m_element->tags(); }
    inline bool pinned() const { return m_element->pinned(); }
    inline bool favorited() const { return m_element->favorited(); }
    inline bool deleted() const { return m_element->deleted(); }
    inline Element* element() const { return m_element; }
    void reload();
    void setupIcons();

private:
    Element* m_element;
};


#endif   // FILEITEM_H
