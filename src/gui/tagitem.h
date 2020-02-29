#ifndef TAGITEM_H
#define TAGITEM_H

#include <QDebug>
#include <QTreeWidgetItem>
#include <QVector>
#include "../element/element.h"

class TagItem : public QTreeWidgetItem {
public:
    TagItem(const QString& label, bool special = false, TagItem* parent = nullptr);
    TagItem(const QString& label, const QString& icon, bool special = false);
    ~TagItem() override;

    inline void addFiles(const QVector<Element*>& els) {
        for (const auto& i : els) addFile(i);
    }
    inline void addFile(Element* element) { m_elements->push_back(element); }
    inline void setLabel(const QString& label) { setText(0, label); }
    inline QVector<Element*>* elements() const { return m_elements; }
    inline QString label() const { return text(0); }
    inline int number() const { return m_elements->size(); }
    inline void remove(Element* e) { m_elements->removeAll(e); }
    inline bool empty() const { return m_elements->isEmpty(); }
    inline bool isSpecial() const { return m_special; }
    bool contains(Element* e);
    void removeElement(Element* element);
    void setColor(const QString& color);
    bool pinned() const { return m_pinned; }
    void setPinned(bool pinned);

private:
    char prevent_padding_warning[4];
    QVector<Element*>* m_elements;
    bool m_special;
    bool m_pinned;
};

#endif   // TAGITEM_H
