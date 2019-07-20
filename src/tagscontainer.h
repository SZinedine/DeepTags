#ifndef TAGSCONTAINER_H
#define TAGSCONTAINER_H

#include <QWidget>
#include <QTreeWidget>
#include "tagitem.h"

// convinient variables
static const std::vector<QString> basicTags = { "All Notes", "Notebooks", "Favorite", "Untagged" };
static const QString& cnv_allNotes 	= basicTags[0];
static const QString& cnv_notebooks = basicTags[1];
static const QString& cnv_favorite 	= basicTags[2];
static const QString& cnv_untagged 	= basicTags[3];

class TagsContainer : public QTreeWidget
{
    Q_OBJECT
public:
    TagsContainer(QWidget* parent=nullptr);

    static inline TagItem* real(QTreeWidgetItem* item)			{	return dynamic_cast<TagItem*>(item);			}
    inline void createBasicTags() 		{ 	for (const QString& i : basicTags) addTopLevelItem( new TagItem(i) ); 	}
    inline void addElements(const ElementsList& elements)		{	for (auto i : elements) addElement( i );		}
    void addElement(Element* element);
    bool alreadyAdded(Element* element);

public slots:
    void init();		// remove all the tags and create the basic ones

signals:
    void itemSelected(QVector<Element*> *item);

private:
    void construct();
    void addToSpecificTopLevel(Element* e, const QString& name);
    int find(const QString& label, QTreeWidgetItem* parent);		// look for an item with a specific label in the children of a parent
    int find(const QString& label, QTreeWidget* parent);
    void pinBasicItems();
    void sort();		// sort and pin the basic items

private:
    QTreeWidgetItem* prnt;
};

#endif // TAGSCONTAINER_H
