#include "tagscontainer.h"
#include "tagitem.h"
#include <QDebug>
#include <iostream>
#include <QAbstractItemView>
#include <QApplication>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDrag>
#include "settings.h"


TagsContainer::TagsContainer(QWidget* parent)
    : QTreeWidget(parent), prnt(nullptr)
{
    construct();
    createBasicTags();
    setMaximumWidth(400);
    setMinimumWidth(50);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(DragDrop);
    setDropIndicatorShown(true);
}

TagsContainer::~TagsContainer() {
    auto lst = real( topLevelItem( find(cnv_allNotes, this) ) )->elements();
    if (lst->isEmpty()) return;
    for (Element* i : *lst)
        if (i) delete i;
}

void TagsContainer::createBasicTags() {
    for (const QString& i : basicTags) 
        addTopLevelItem( new TagItem(i) ); 
}

void TagsContainer::init() {
    clear();
    createBasicTags();
}

void TagsContainer::construct() {
    connect(this, &TagsContainer::itemClicked,
            this, [=](){  selected();  });
}


void TagsContainer::selected() {
    QList<QTreeWidgetItem*> lst = selectedItems();
    if (lst.isEmpty())           return;
    else if (lst.size() == 1)    emit itemSelected( real(lst.at(0))->elements() );
    if(lst.isEmpty() || lst.size() == 1) return;

    auto shared = [](const QList<QTreeWidgetItem*>& items, Element* el) -> bool {
        for (QTreeWidgetItem* i : items)
            if ( !real(i)->contains(el) ) return false;
        return true;
    };
    auto *res = new QVector<Element*>();
    QVector<Element*> *first = real(lst.at(0))->elements();
    for (Element* e : *first)
        if (shared(lst, e)) res->push_back(e);

    emit itemSelected(res);
    delete res;
}


int TagsContainer::find(const QString &label, QTreeWidgetItem* parent) {
    for (int i = 0 ; i < parent->childCount() ; i++)
        if( (real(parent->child(i)))->label() == label ) 
            return i;  // index of the found element
    return -1;   // it doesn't exist
}


int TagsContainer::find(const QString &label, QTreeWidget *parent) {
    for (int i = 0 ; i < parent->topLevelItemCount() ; i++) {
        if ( (real(parent->topLevelItem(i)))->label() == label) 
            return i;
    }
    return -1;
}


void TagsContainer::addElements(const ElementsList& elements) {
    emit loadingFiles();
    int i = 0;
    for (Element* e : elements ) {
        addElement( e );

        i++;
        if (i%500 == 0) {
            sort();
            qApp->processEvents();
        }
    }
    sort();
    emit filesLoaded();
}


void TagsContainer::addElement(Element* element) {
/**
  * retrieve the tags of the Element. all the work will be based on them
  * add the element to "All Notes" tag.
  * add it to untagged if it hasn't any tags.
  * add it to favorite if is favorited.
  *
  * iterate through the Tags (vector of vectors)
  * single tag treatment:
  *     * create a TagItem if it doesn't exist (index == -1)
  *         * add it to topLevel if level == 0
  *         * add it to prnt (previous parent) if level != 0
  *     * append the entire Element to another item if it exists
  *         * find the item to append to (by using the level variable)
  *         * verify if the found item doesn't contain the Element.
  *             * append it to it if it doesn't
*/

    if (!element) return;
    if (element->deleted()) {
        toTrash(element);
        return;
    }
    if (alreadyAdded(element)) return;
    addToSpecificTopLevel(element, cnv_allNotes);    // add to All Notes if it isn't there

    const Tags& tags = element->tags();

    if (element->favorited()) addToSpecificTopLevel(element, cnv_favorite);
    if (tags.empty()) {
        addToSpecificTopLevel(element, cnv_untagged);
        return;
    }

    for (const StringList& chain : tags) {
        for (std::string::size_type level = 0 ; level < chain.size() ; level++) {

            const QString particle( chain.at(level).c_str() ); // item name to be treated
            int index = (level == 0) ? find(particle, this) : find(particle, prnt); // index 0 == search fo a top level 

            switch (index) {
            case -1: {            // create the tag
                auto *newItem = new TagItem(particle);
                newItem->addFile(element);
                if (level == 0) addTopLevelItem(newItem);
                else            prnt->addChild(newItem);
                prnt = newItem;
                break;
            }
            default:            // append Element to an existing tag
                // grab a topLevelItem or a sub Item depending on the level of deepness (i variable)
                QTreeWidgetItem* foundItem = (level == 0) ? topLevelItem(index) : prnt->child(index);
                TagItem* castedItem = real(foundItem);
                if (!castedItem->contains(element))
                    castedItem->addFile(element);
                prnt = foundItem;
                break;
            }
        }
        prnt = nullptr;
    }
}


void TagsContainer::addToSpecificTopLevel(Element *e, const QString &name) {
    int index = find(name, this);
    if (index == -1) return;        // abort if the item isn't found
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
    return (real(topLevelItem( 0 )))->contains(element);        // search inside All notes
}


void TagsContainer::restoreElement(Element* element) {
    element->changeDeleted(false);
    pullElement(element);
    addElements( {element} );
    removeEmptyItems();
}

void TagsContainer::reloadElement(Element* element) {
    element->reload();
    pullElement(element);
    ElementsList es;
    es.push_back(element);
    addElements(es);
}


void TagsContainer::removeEmptyItems() {
    QTreeWidgetItemIterator it(this);
    while (*it) {
        TagItem* current = real(*it);
        if (current->empty() && !current->isSpecial())
            delete current;
        it++;
    }
}

void TagsContainer::toTrash(Element *element) {
    int index = find(tr("Trash"), this);
    TagItem *trash;

    if (index == -1) {
        trash = new TagItem(tr("Trash"));
        addTopLevelItem(trash);
    }
    else
        trash = real(topLevelItem(index));

    trash->addFile(element);
}


void TagsContainer::permatentlyDelete(Element* element) {
    pullElement(element);
    QString file(element->path().string().c_str());
    delete element;
    QFile::remove(file);
}

void TagsContainer::pullElement(Element *element) {
    QTreeWidgetItemIterator it(this);
    while (*it) {
        (real(*it))->removeElement(element);
        it++;
    }
    removeEmptyItems();
}

void TagsContainer::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText())
        event->accept();
    else
        event->ignore();
}


void TagsContainer::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void TagsContainer::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void TagsContainer::startDrag(Qt::DropActions /*supportedActions*/) {
    TagItem *item = real(currentItem());
    if (!item) return;

    TagItem* i = item;
    QStringList strl;
    while (true) {
        if (!i) break;
        strl.prepend(i->label());
        i = real(i->parent());
    }
    std::vector<std::string> str;
    for (const auto &i : strl) str.push_back(i.toStdString());
    QString s = QString::fromStdString( Element::combineTags(str) );

    auto *mimeData = new QMimeData;
    mimeData->setText(s);

    auto *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}



void TagsContainer::collapseItems() {
    collapseAll();
    Settings::expand(false);
}


void TagsContainer::expandItems() {
    expandAll();
    Settings::expand(true);
}


void TagsContainer::loadCollapseOrExpand() {
    if (Settings::expandedItems()) expandAll();
    else collapseAll();
}



