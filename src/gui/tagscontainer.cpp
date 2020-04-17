#include "tagscontainer.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QDebug>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>
#include <memory>
#include "settings.h"
#include "tagitem.h"

TagsContainer::TagsContainer(QWidget* parent) : QTreeWidget(parent), prnt(nullptr) {
    connect(this, &TagsContainer::itemClicked, this, [=]() { selected(); });
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
    deleteAllItems();
}

void TagsContainer::deleteAllItems() {
    auto lst = real(topLevelItem(find(cnv_allNotes, this)))->elements();
    if (lst->isEmpty()) return;
    for (Element* i : *lst)
        if (i) delete i;

    QTreeWidgetItemIterator it(this);
    while (*it) {
        delete real(*it);
        it++;
    }
    clear();
}

void TagsContainer::createBasicTags() {
    QStringList basicIcons{ ":images/all_notes.png", ":images/notebook.png", ":images/star.png",
                            ":images/untagged.png" };
    for (std::vector<QString>::size_type i = 0; i < basicTags.size(); i++)
        addTopLevelItem(new TagItem(basicTags[i], basicIcons[i], true));
    applyColors();
}

void TagsContainer::init() {
    deleteAllItems();
    clear();
    createBasicTags();
}


void TagsContainer::selected() {
    QList<QTreeWidgetItem*> lst = selectedItems();
    if (lst.isEmpty()) return;
    if (lst.size() == 1) {
        emit itemSelected(real(lst.at(0))->elements());
        return;
    }

    // check if the different TagItems contain a specific element
    auto shared = [lst](Element* el) -> bool {
        for (QTreeWidgetItem* i : lst)
            if (!real(i)->contains(el)) return false;
        return true;
    };

    QVector<Element*> res;
    QVector<Element*>* first = real(lst.at(0))->elements();
    for (Element* e : *first)
        if (shared(e)) res.push_back(e);

    emit itemSelected(&res);
}


int TagsContainer::find(const QString& label, QTreeWidgetItem* parent) {
    for (int i = 0; i < parent->childCount(); i++)
        if ((real(parent->child(i)))->label() == label) return i;   // index of the found element
    return -1;                                                      // it doesn't exist
}


int TagsContainer::find(const QString& label, QTreeWidget* parent) {
    for (int i = 0; i < parent->topLevelItemCount(); i++)
        if ((real(parent->topLevelItem(i)))->label() == label) return i;
    return -1;
}


void TagsContainer::addElements(const ElementsList& elements) {
    emit loadingFiles();
    int i = 0;
    for (Element* e : elements) {
        addElement(e);

        i++;
        if (i % 500 == 0) {
            sort();
            qApp->processEvents();
            applyColors();
        }
    }
    sort();
    applyColors();
    emit filesLoaded();
}


void TagsContainer::addElement(Element* element) {
    /**
     * TODO: THIS FUNCTION NEEDS TO BE OPTIMIZED. IT IS TOO SLOW AND TOO COMPLICATED
     *
     * Check if the element is deleted, if it is, append it to trash and return.
     * add to the untagged/favorited if it requires so.
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
    addToSpecificTopLevel(element, cnv_allNotes);   // add to All Notes if it isn't there

    if (element->favorited()) addToSpecificTopLevel(element, cnv_favorite);
    if (element->untagged()) {
        addToSpecificTopLevel(element, cnv_untagged);
        return;
    }

    const Tags& tags = element->tags();

    for (const StringList& chain : tags) {
        for (std::string::size_type level = 0; level < chain.size(); level++) {
            const QString particle(chain.at(level));   // item name to be treated
            int index = (level == 0) ? find(particle, this)
                                     : find(particle, prnt);   // index 0 == search fo a top level

            switch (index) {
            case -1: {   // create the tag
                auto* newItem = new TagItem(particle);
                newItem->addFile(element);
                if (level == 0)
                    addTopLevelItem(newItem);
                else
                    prnt->addChild(newItem);
                prnt = newItem;
                break;
            }
            default:   // append Element to an existing tag
                // grab a topLevelItem or a child Item depending on the level of deepness (level
                // variable)
                QTreeWidgetItem* foundItem =
                    (level == 0) ? topLevelItem(index) : prnt->child(index);
                TagItem* castedItem = real(foundItem);
                if (!castedItem->contains(element)) castedItem->addFile(element);
                prnt = foundItem;
                break;
            }
        }
        prnt = nullptr;
    }
}


void TagsContainer::addToSpecificTopLevel(Element* e, const QString& name) {
    int index = find(name, this);
    if (index == -1) return;   // abort if the item isn't found
    real(topLevelItem(index))->addFile(e);
}


void TagsContainer::pinBasicItems() {
    for (auto i = basicTags.rbegin(); i != basicTags.rend(); i++)
        insertTopLevelItem(0, takeTopLevelItem(find(*i, this)));
}


void TagsContainer::sort() {
    sortByColumn(0, Qt::AscendingOrder);
    pinTags();
    // pinBasicItems();
}

bool TagsContainer::alreadyAdded(Element* element) {
    return (real(topLevelItem(0)))->contains(element);   // search inside All notes
}


void TagsContainer::restoreElement(Element* element) {
    element->changeDeleted(false);
    pullElement(element);
    addElements({ element });
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
        if (current->empty()) {
            if (current->isSpecial() && current->label() == "Trash")
                delete current;
            else if (!current->isSpecial())
                delete current;
        }
        it++;
    }
}

void TagsContainer::toTrash(Element* element) {
    int index = find(tr("Trash"), this);
    TagItem* trash;

    if (index == -1) {
        trash = new TagItem(tr("Trash"), ":images/trash.png", true);
        addTopLevelItem(trash);
    } else
        trash = real(topLevelItem(index));

    if (!trash->contains(element)) trash->addFile(element);
}


void TagsContainer::permatentlyDelete(Element* element) {
    pullElement(element);
    QString file(element->path());
    delete element;
    QFile::remove(file);
}

void TagsContainer::pullElement(Element* element) {
    QTreeWidgetItemIterator it(this);
    while (*it) {
        (real(*it))->removeElement(element);
        it++;
    }
    removeEmptyItems();
}

void TagsContainer::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasText())
        event->accept();
    else
        event->ignore();
}


void TagsContainer::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else
        event->ignore();
}


void TagsContainer::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else
        event->ignore();
}


void TagsContainer::startDrag(Qt::DropActions /*supportedActions*/) {
    TagItem* item = real(currentItem());
    if (!item) return;

    TagItem* i = item;
    QStringList strl;
    while (true) {
        if (!i) break;
        strl.prepend(i->label());
        i = real(i->parent());
    }
    std::vector<QString> str;
    for (const auto& i : strl) str.push_back(i);
    QString s = be::combineTags(str);

    auto* mimeData = new QMimeData;
    mimeData->setText(s);

    auto* drag = new QDrag(this);
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
    if (Settings::expandedItems())
        expandAll();
    else
        collapseAll();
}

void TagsContainer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        QPoint qp(event->pos());
        setCurrentItem(itemAt(qp));
        showContextMenu(qp);
    } else
        QTreeWidget::mousePressEvent(event);
}

void TagsContainer::showContextMenu(QPoint pos) {
    QTreeWidgetItem* item = itemAt(pos);
    if (!item) return;
    if (item->parent()) return;   // only topLevel items
    TagItem* it = real(item);

    auto menu      = std::make_unique<QMenu>();
    auto colorMenu = std::make_unique<QMenu>(tr("Change the color"), this);
    auto def       = std::make_unique<QAction>(tr("default color"), this);
    auto green     = std::make_unique<QAction>(QIcon(":images/color_green"), tr("green"), this);
    auto yellow    = std::make_unique<QAction>(QIcon(":images/color_yellow"), tr("yellow"), this);
    auto orange    = std::make_unique<QAction>(QIcon(":images/color_orange"), tr("orange"), this);
    auto red       = std::make_unique<QAction>(QIcon(":images/color_red"), tr("red"), this);
    auto magenta   = std::make_unique<QAction>(QIcon(":images/color_magenta"), tr("magenta"), this);
    auto blue      = std::make_unique<QAction>(QIcon(":images/color_blue"), tr("blue"), this);
    auto cyan      = std::make_unique<QAction>(QIcon(":images/color_cyan"), tr("cyan"), this);
    colorMenu->addActions({ def.get(), green.get(), yellow.get(), orange.get(), red.get(),
                            magenta.get(), blue.get(), cyan.get() });
    menu->addMenu(colorMenu.get());

    if (!it->isSpecial()) {
        auto piAction = menu->addAction(((it->pinned()) ? "Unpin" : "Pin"));
        connect(piAction, &QAction::triggered, [&] {
            it->setPinned(!(it->pinned()));
            sort();
        });
    }

    connect(def.get(), &QAction::triggered, this, [it] { it->setColor(""); });
    connect(red.get(), &QAction::triggered, this, [it] { it->setColor("red"); });
    connect(green.get(), &QAction::triggered, this, [it] { it->setColor("green"); });
    connect(blue.get(), &QAction::triggered, this, [it] { it->setColor("blue"); });
    connect(yellow.get(), &QAction::triggered, this, [it] { it->setColor("yellow"); });
    connect(magenta.get(), &QAction::triggered, this, [it] { it->setColor("magenta"); });
    connect(cyan.get(), &QAction::triggered, this, [it] { it->setColor("cyan"); });
    connect(orange.get(), &QAction::triggered, this, [it] { it->setColor("orange"); });

    menu->exec(mapToGlobal(pos));
}

void TagsContainer::applyColors() {
    auto map = Settings::getTagItemColor();
    for (auto i = map.begin(); i != map.end(); i++) {
        int row = find(i.key(), this);
        if (row == -1) continue;
        auto it = real(topLevelItem(row));
        it->setColor(i.value().toString());
    }
}


void TagsContainer::pinTags() {
    QStringList lst = Settings::getTagPinned();
    lst.sort(Qt::CaseInsensitive);
    // for (auto i = lst.rbegin(); i != lst.rend(); i++) {
    for (auto i = lst.end() - 1; i >= lst.begin(); i--) {
        int row = find(*i, this);
        if (row == -1) continue;
        auto it = real(takeTopLevelItem(row));
        it->setPinned(true);
        insertTopLevelItem(0, it);
    }
    pinBasicItems();
}
