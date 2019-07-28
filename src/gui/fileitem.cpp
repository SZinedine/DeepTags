#include "fileitem.h"
#include "filescontainer.h"

FileItem::FileItem(FilesContainer* parent) : QListWidgetItem("", parent, 1600) {}

FileItem::FileItem(Element* element, FilesContainer* parent)
    : QListWidgetItem(QString::fromStdString(element->title()), parent, 1600)
{
    setElement(element);
}

void FileItem::setElement(Element *element) {
    m_element = element;
    setLabel(m_element->title());
}
