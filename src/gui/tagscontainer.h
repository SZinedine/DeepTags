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
    ~TagsContainer() override;

    static inline TagItem* real(QTreeWidgetItem* item)			{	return dynamic_cast<TagItem*>(item);			}
    inline void createBasicTags() 		{ 	for (const QString& i : basicTags) addTopLevelItem( new TagItem(i) ); 	}
    void addElements(const ElementsList& elements);
    bool alreadyAdded(Element* element);
    void selected();			// if multiple tags are selected, construct a list of Elements they share and send it to FilesContainer

public slots:
    /**
     * remove all the tags and create the basic ones
     */
    void init();
    void removeElement(Element* element);
    void reloadElement(Element* element);
    void removeEmptyItems();
    void collapseItems();								// remember if collapsed/expanded
    void expandItems();
    void loadCollapseOrExpand();

signals:
    void itemSelected(QVector<Element*> *item);			// when one or multiple tags are selected, send their content to be displayed
    void loadingFiles();								// emited when a list of Elements are loading
    void filesLoaded();									// emited when a list of Elements finished loading
    void itemAdded();									// emited when an Element is processed

private:
    void construct();
    void addElement(Element* element);					// create a tag(s) from Element, or append the Element to existing tags
    void addToSpecificTopLevel(Element* e, const QString& name);
    int find(const QString& label, QTreeWidgetItem* parent);		// look for an item with a specific label in the children of a parent
    int find(const QString& label, QTreeWidget* parent);
    void pinBasicItems();
    void sort();		// sort and pin the basic items
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

private:
    QTreeWidgetItem* prnt;		// keep track on the previous parent when the items are added
};

#endif // TAGSCONTAINER_H
