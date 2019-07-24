#include "tagitem.h"

TagItem::TagItem() : QTreeWidgetItem(QStringList(""), 1500) { m_elements = new QVector<Element*>(); }

TagItem::TagItem(const QString& label, TagItem* parent)
    : QTreeWidgetItem(parent, QStringList(label), 1500)
{
    setLabel(label);
    m_elements = new QVector<Element*>();
}


void TagItem::removeElement(Element *element) {
    for (int i = 0 ; i < m_elements->size() ; i++)
        if (*m_elements->at(i) == *element)
            m_elements->remove(i);
}
