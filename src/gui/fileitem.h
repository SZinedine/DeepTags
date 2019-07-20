#ifndef FILEITEM_H
#define FILEITEM_H

#include <QListWidgetItem>
#include "../back/element.h"

class FilesContainer;

class FileItem : public QListWidgetItem
{
public:
    FileItem(FilesContainer* parent=nullptr);
    FileItem(Element* element, FilesContainer* parent=nullptr);

    void setElement(Element* element);
    inline fs::path path() const		{	return m_element->path(); 										}
    inline QString pathQstr() const		{	return QString::fromStdString(m_element->path().c_str()); 		}
    inline QString title() const		{	return QString::fromStdString( m_element->title() ); 			}
    inline Tags tags() const			{	return m_element->tags();										}
    inline bool pinned() const			{	return m_element->pinned();										}
    inline bool favorited() const		{	return m_element->favorited();									}
    inline Element* element() const		{	return m_element;												}

private:
    char prevent_padding_warning[4];
    Element* m_element;
};

#endif // FILEITEM_H
