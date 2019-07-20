#ifndef TAGITEM_H
#define TAGITEM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "../back/element.h"
#include <QDebug>
#include <QVector>

class TagItem : public QTreeWidgetItem
{
public:
    TagItem();
    TagItem(const QString& label, TagItem* parent=nullptr);

    void addFile(Element* element);
    inline void addFiles(QVector<Element*> els) {	for (auto i : els) addFile(i);	}
    inline void setLabel(const QString& label)	{	setText(0, label);				}
    inline QVector<Element*>* elements() 		{	return m_elements;				}
    inline QString label() const				{	return text(0);					}
    inline int number() const					{	return m_elements->size();		}
    inline void remove(Element* e)				{	m_elements->removeAll(e);		}
    bool contains(Element* e) const;
    void removeElement(Element* element);

private:
    char prevent_padding_warning[4];
    QVector<Element*> *m_elements;
};

#endif // TAGITEM_H
