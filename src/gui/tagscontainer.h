#ifndef TAGSCONTAINER_H
#define TAGSCONTAINER_H

#include <QTreeWidget>
#include <QWidget>
#include "tagitem.h"

// convinient variables
static const std::vector<QString> basicTags = { "All Notes", "Notebooks", "Favorite", "Untagged" };
static const QString& cnv_allNotes          = basicTags[0];
static const QString& cnv_notebooks         = basicTags[1];
static const QString& cnv_favorite          = basicTags[2];
static const QString& cnv_untagged          = basicTags[3];

class TagsContainer : public QTreeWidget {
    Q_OBJECT
public:
    TagsContainer(QWidget* parent = nullptr);
    ~TagsContainer() override;
    static inline TagItem* real(QTreeWidgetItem* item) { return static_cast<TagItem*>(item); }
    void createBasicTags();
    void addElements(const ElementsList& elements);
    bool alreadyAdded(Element* element);
    /**
     *  if multiple tags are selected, construct a list of
     *  Elements they share and send it to FilesContainer
     *  By emitting a signal (itemSelected)
     */
    void selected();
    /**
     * remove all the tags and create the basic ones
     */
    void init();
    void restoreElement(Element* element);
    void reloadElement(Element* element);
    void removeEmptyItems();
    void collapseItems();   // remember if collapsed/expanded
    void expandItems();
    void loadCollapseOrExpand();
    void permatentlyDelete(Element* element);
    void deleteAllItems();

private:
    /**
     *  create a tag(s) from Element,
     *  or append the Element to existing tags
     */
    void addElement(Element* element);
    void addToSpecificTopLevel(Element* e, const QString& name);
    /**
     *  look for an item with a specific label in the children of a parent
     */
    int find(const QString& label, QTreeWidgetItem* parent);
    int find(const QString& label, QTreeWidget* parent);
    void pinBasicItems();
    void sort();   // sort and pin the basic items
    void toTrash(Element* element);
    void pullElement(Element* element);
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void mousePressEvent(QMouseEvent* event) override;
    void showContextMenu(QPoint pos);
    void applyColors();
    void pinTags();

signals:
    /**
     *  when one or multiple tags are selected,
     *  send their content to be displayed
     */
    void itemSelected(QVector<Element*>* item);
    /**
     *  emited when a list of Elements are loading
     */
    void loadingFiles();
    /**
     *  emited when a list of Elements finished loading
     */
    void filesLoaded();

private:
    QTreeWidgetItem* prnt;   // keep track on the previous parent when the items are added
};

#endif   // TAGSCONTAINER_H
