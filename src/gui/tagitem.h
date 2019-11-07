#ifndef TAGITEM_H
#define TAGITEM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "../element/element.h"
#include <QDebug>
#include <QVector>

class TagItem : public QTreeWidgetItem
{
public:
    TagItem(const QString& label="", TagItem* parent=nullptr);
    ~TagItem() override;

    inline void addFiles(const QVector<Element*>& els) { for (const auto& i : els) addFile(i);}
    inline void addFile(Element* element)      { m_elements->push_back(element);  }
    inline void setLabel(const QString& label) { setText(0, label);               }
    inline QVector<Element*>* elements() const { return m_elements;               }
    inline QString label() const               { return text(0);                  }
    inline int number() const                  { return m_elements->size();       }
    inline void remove(Element* e)             { m_elements->removeAll(e);        }
    inline bool empty() const                  { return m_elements->isEmpty();    }
    bool isSpecial() const;
    bool contains(Element* e);
    void removeElement(Element* element);

private:
    char prevent_padding_warning[4];
    QVector<Element*> *m_elements;
};

#endif // TAGITEM_H
