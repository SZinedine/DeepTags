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
#include "fileitem.h"
#include "filescontainer.h"

FileItem::FileItem(FilesContainer* parent)
    : QListWidgetItem("", parent, 1600), m_element(nullptr) {}

FileItem::FileItem(Element* element, FilesContainer* parent)
    : QListWidgetItem(element->title(), parent, 1600), m_element(element) {
    reload();
}

void FileItem::setElement(Element* element) {
    m_element = element;
    reload();
}


void FileItem::reload() {
    if (!m_element) return;
    setLabel(m_element->title());
    setupIcons();
}

void FileItem::setupIcons() {
    const bool& p = element()->pinned();
    const bool& f = element()->favorited();
    if (p && f)
        setIcon(QIcon(favPinIcon));
    else if (p)
        setIcon(QIcon(pinnedIcon));
    else if (f)
        setIcon(QIcon(favoritedIcon));
    else
        setIcon(QIcon());
}
