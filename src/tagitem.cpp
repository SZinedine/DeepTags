#include "tagitem.h"

TagItem::TagItem() : QTreeWidgetItem(QStringList(""), 1500) { m_elements = new QVector<Element*>(); }

TagItem::TagItem(const QString& label, TagItem* parent)
    : QTreeWidgetItem(parent, QStringList(label), 1500)
{
    setLabel(label);
    m_elements = new QVector<Element*>();
}

void TagItem::addFile(Element *element) {
    m_elements->push_back(element);
}

bool TagItem::contains(Element* e) const {
    for(Element* i : *m_elements)
        if (*e == *i) return true;
    return false;
}
