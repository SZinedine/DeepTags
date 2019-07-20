#include "tagscontainer.h"
#include "tagitem.h"
#include <QDebug>
#include <iostream>

TagsContainer::TagsContainer(QWidget* parent)
    :QTreeWidget(parent)
{
    construct();
    createBasicTags();
    prnt = nullptr;
    setMaximumWidth(250);
}

void TagsContainer::init() {
    clear();
    createBasicTags();
}

void TagsContainer::construct() {
    connect(this, &TagsContainer::itemClicked,				// display the files in the FilesContainer when an item is clicked
            this, [=](QTreeWidgetItem *item){
        emit(  (itemSelected( real(item)->elements() ))  );		// QVector<Element*>*
    });
}

int TagsContainer::find(const QString &label, QTreeWidgetItem* parent) {
    for (int i = 0 ; i < parent->childCount() ; i++) {
        const TagItem* item = real(parent->child(i));
        if( item->label() == label ) return i;
    }
    return -1;
}


int TagsContainer::find(const QString &label, QTreeWidget *parent) {
    for (int i = 0 ; i < parent->topLevelItemCount() ; i++) {
        TagItem* item = real(parent->topLevelItem(i));
        if (item->label() == label) return i;
    }
    return -1;
}


void TagsContainer::addElement(Element* element) {
    /*
     * retrieve Tags from the element
     * iterate thtough the Tags :
     * 		if the first doesn't exist, create it and append the element into it.
     *  	else append the element into it.
     * continue the iteration by verifying if the following tag exist...
     *
     */

    if (alreadyAdded(element)) {
        std::cerr << "already exist: " << element->path() << "\n";
        return;
    }
    const Tags& tags = element->tags();

    addToSpecificTopLevel(element, cnv_allNotes);
    if (element->favorited())	addToSpecificTopLevel(element, cnv_favorite);
    if (tags.empty()) addToSpecificTopLevel(element, cnv_untagged);

    for (const StringList& chain : tags) {

        for (std::string::size_type level = 0 ; level < chain.size() ; level++) {
            const QString particle = QString(chain.at(level).c_str());
            int index = (level == 0) ? find(particle, this) : find(particle, prnt);

            switch (index) {
            case -1: {
                TagItem *newItem = new TagItem(particle);
                newItem->addFile(element);
                if (level == 0)	addTopLevelItem(newItem);
                else		prnt->addChild(newItem);
                prnt = newItem;
                break;
            }
            default:
                // grab a topLevelItem or a sub Item depending on the level of deepness (i variable)
                QTreeWidgetItem* foundItem = (level == 0) ? topLevelItem(index) : prnt->child(index);
                TagItem* castedItem = real(foundItem);
                castedItem->addFile(element);
                prnt = foundItem;
                break;
            }
        }
        prnt = nullptr;
    }
    sort();
}

void TagsContainer::addToSpecificTopLevel(Element *e, const QString &name) {
    int index = find(name, this);
    if (index == -1) return;		// abort if the item isn't found
    TagItem* it = real(topLevelItem(index));
    it->addFile(e);
}


void TagsContainer::pinBasicItems() {
    for (auto i = basicTags.rbegin() ; i != basicTags.rend() ; i++)
        insertTopLevelItem(0, takeTopLevelItem(find(*i, this)));
}


void TagsContainer::sort() {
    sortByColumn(0, Qt::AscendingOrder);
    pinBasicItems();
}

bool TagsContainer::alreadyAdded(Element* element) {
    TagItem* item = real(topLevelItem( find(basicTags[0], this) ));
    return item->contains(element);
}



