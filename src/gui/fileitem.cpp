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
