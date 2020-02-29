#ifndef FILEITEM_H
#define FILEITEM_H

#include <QListWidgetItem>
#include "../element/element.h"

class FilesContainer;
const QString pinnedIcon    = ":images/pin.png";
const QString favoritedIcon = ":images/favorite.png";
const QString favPinIcon    = ":images/favpin.png";

class FileItem : public QListWidgetItem {
public:
    FileItem(FilesContainer* parent = nullptr);
    FileItem(Element* element, FilesContainer* parent = nullptr);

    void setElement(Element* element);
    inline void setLabel(const QString& label) { setText(label); }
    inline void setLabel(const std::string& label) { setText(QString::fromStdString(label)); }
    inline fs::path path() const { return m_element->path(); }
    inline QString pathQstr() const { return QString(m_element->path().string().c_str()); }
    inline QString title() const { return QString(m_element->title().c_str()); }
    inline Tags tags() const { return m_element->tags(); }
    inline bool pinned() const { return m_element->pinned(); }
    inline bool favorited() const { return m_element->favorited(); }
    inline Element* element() const { return m_element; }
    void reload();
    void setupIcons();

private:
    char prevent_padding_warning[4];
    Element* m_element;
};


#endif   // FILEITEM_H
