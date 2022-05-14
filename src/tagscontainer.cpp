/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2022  Zineddine Saibi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/
#include "tagscontainer.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>
#include <QSet>
#include <QTreeWidget>
#include <memory>
#include "settings.h"
#include "tagitem.h"

TagsContainer::TagsContainer(QWidget* parent) : QTreeWidget(parent) {
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
    auto lst = find(cnv_allNotes)->allElements();
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
        new TagItem(basicTags[i], basicIcons[i], true, this);
    applyColors();
}

void TagsContainer::init() {
    deleteAllItems();
    clear();
    createBasicTags();
}

void TagsContainer::selected() {
    QList<TagItem*> lst;
    for (auto& i : selectedItems()) lst.push_back(real(i));
    if (lst.isEmpty()) return;

    // check if the different TagItems contain a specific element
    auto sharedElement = [&](Element* e) -> bool {
        for (auto& i : lst)
            if (!i->contains(e)) return false;
        return true;
    };

    QSet<Element*> set;
    for (auto& l : lst) {
        auto elements = l->allElements();
        for (auto e : *elements)
            if (sharedElement(e)) set += e;
        delete elements;
    }
    auto res = set.values();
    itemSelected(&res);
}

void TagsContainer::selectedExclusive() {
    auto it = real(currentItem());
    if (!it) return;
    emit itemSelected(it->elements());
}

TagItem* TagsContainer::find(const QString& label, TagItem* parent) {
    if (parent) {   // find an item which is a child of another item
        for (int i = 0; i < parent->childCount(); i++) {
            auto child = parent->child(i);
            if (child->text(0) == label) {
                return real(child);
            }
        }
        return nullptr;   // not found in the TagItem
    } else {              // find a top level item
        QTreeWidgetItemIterator i(this);
        while (*i) {
            auto child = *i;
            if (child->text(0) == label) {
                return real(child);
            }
            i++;
        }
    }
    return nullptr;   // not found at all
}

void TagsContainer::addElements(const ElementsList& elements) {
    emit loadingFiles();
    for (Element* e : elements) {
        addElement(e);
    }
    sort();
    applyColors();
    emit filesLoaded();
}

void TagsContainer::addElement(Element* element) {
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

    std::function<void(const StringList&, StringList::size_type, TagItem*)> constructChain;
    constructChain = [&](const StringList& tags, StringList::size_type index, TagItem* parent) {
        if (tags.size() == index) return;
        const QString& current_particle = tags.at(index);
        TagItem* current_item           = nullptr;

        if (parent) {
            current_item = find(current_particle, parent);
            if (!current_item) current_item = new TagItem(current_particle, parent);
        } else {   // toplevel
            current_item = find(current_particle);
            if (!current_item) current_item = new TagItem(current_particle, this);
        }

        if (tags.size() - 1 == index)   // only add the Element* to the last tag particle in a chain
            current_item->addFile(element);

        constructChain(tags, index + 1, current_item);
    };

    for (const StringList& sl : element->tags()) constructChain(sl, 0, nullptr);
}

void TagsContainer::addToSpecificTopLevel(Element* e, const QString& name) {
    auto it = find(name);
    if (it) it->addFile(e);
}

void TagsContainer::pinBasicItems() {
    for (auto i = basicTags.rbegin(); i != basicTags.rend(); i++)
        insertTopLevelItem(0, takeTopLevelItem(indexOfTopLevelItem(find(*i))));
}

void TagsContainer::sort() {
    sortByColumn(0, Qt::AscendingOrder);
    pinTags();
}

bool TagsContainer::alreadyAdded(Element* element) {
    return (real(topLevelItem(0)))->contains(element);   // search inside All notes
}

void TagsContainer::restoreElement(Element* element) {
    element->changeDeleted(false);
    pullElement(element);
    addElements({ element });
    removeEmptyItems();
    removeEmptyItems();
}

void TagsContainer::reloadElement(Element* element) {
    element->reload();
    pullElement(element);
    ElementsList es;
    es.push_back(element);
    addElements(es);
}

void TagsContainer::removeEmptyItems() {   // sometimes, this should be called 2 times in order to
                                           // remove all the empty items
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
    TagItem* trash = find(tr("Trash"));

    if (!trash) {
        trash = new TagItem(tr("Trash"), ":images/trash.png", true, this);
        addTopLevelItem(trash);
    }

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
    delete drag;
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
    TagItem* it = real(item);

    auto menu          = std::make_unique<QMenu>();
    auto displayAction = std::make_unique<QAction>(tr("Display"), menu.get());
    auto displayExclusiveAction =
        std::make_unique<QAction>(tr("Display without children's elements"), menu.get());
    auto copyTag = std::make_unique<QAction>(tr("Copy Tag"), menu.get());

    auto colorMenu = std::make_unique<QMenu>(tr("Change the color"), menu.get());
    auto def       = std::make_unique<QAction>(tr("default color"), menu.get());
    auto green = std::make_unique<QAction>(QIcon(":images/color_green"), tr("green"), menu.get());
    auto yellow =
        std::make_unique<QAction>(QIcon(":images/color_yellow"), tr("yellow"), menu.get());
    auto orange =
        std::make_unique<QAction>(QIcon(":images/color_orange"), tr("orange"), menu.get());
    auto red = std::make_unique<QAction>(QIcon(":images/color_red"), tr("red"), menu.get());
    auto magenta =
        std::make_unique<QAction>(QIcon(":images/color_magenta"), tr("magenta"), menu.get());
    auto blue = std::make_unique<QAction>(QIcon(":images/color_blue"), tr("blue"), menu.get());
    auto cyan = std::make_unique<QAction>(QIcon(":images/color_cyan"), tr("cyan"), menu.get());
    colorMenu->addActions({ def.get(), green.get(), yellow.get(), orange.get(), red.get(),
                            magenta.get(), blue.get(), cyan.get() });
    std::unique_ptr<QAction> expandOrCollapseAction;

    menu->addAction(displayAction.get());
    if (item->childCount()) menu->addAction(displayExclusiveAction.get());
    if (!item->parent()) menu->addMenu(colorMenu.get());
    if (!it->isSpecial() && !it->parent()) {
        auto piAction = menu->addAction(((it->pinned()) ? "Unpin" : "Pin"));
        connect(piAction, &QAction::triggered, [&] {
            it->setPinned(!(it->pinned()));
            sort();
        });
    }
    if (!it->isSpecial()) menu->addAction(copyTag.get());
    if (it->hasChildren()) {
        const bool exp = it->isExpanded();
        expandOrCollapseAction =
            std::make_unique<QAction>((exp) ? tr("Collapse") : tr("Expand"), menu.get());
        connect(expandOrCollapseAction.get(), &QAction::triggered, this,
                [it, exp] { it->setExpanded(!exp); });
        menu->addAction(expandOrCollapseAction.get());
    }

    connect(def.get(), &QAction::triggered, this, [it] { it->setColor(""); });
    connect(red.get(), &QAction::triggered, this, [it] { it->setColor("red"); });
    connect(green.get(), &QAction::triggered, this, [it] { it->setColor("green"); });
    connect(blue.get(), &QAction::triggered, this, [it] { it->setColor("blue"); });
    connect(yellow.get(), &QAction::triggered, this, [it] { it->setColor("yellow"); });
    connect(magenta.get(), &QAction::triggered, this, [it] { it->setColor("magenta"); });
    connect(cyan.get(), &QAction::triggered, this, [it] { it->setColor("cyan"); });
    connect(orange.get(), &QAction::triggered, this, [it] { it->setColor("orange"); });
    connect(displayAction.get(), &QAction::triggered, this, [=] { selected(); });
    connect(displayExclusiveAction.get(), &QAction::triggered, this, [=] { selectedExclusive(); });
    connect(copyTag.get(), &QAction::triggered, this,
            [it] { qApp->clipboard()->setText(it->completeTag()); });

    menu->exec(mapToGlobal(pos));
}

void TagsContainer::applyColors() {
    auto map = Settings::getTagItemColor();
    for (auto i = map.begin(); i != map.end(); i++) {
        auto it = find(i.key());
        if (it) it->setColor(i.value().toString());
    }
}

void TagsContainer::pinTags() {
    QStringList lst = Settings::getTagPinned();
    lst.sort(Qt::CaseInsensitive);
    // for (auto i = lst.rbegin(); i != lst.rend(); i++) {
    for (auto i = lst.end() - 1; i >= lst.begin(); i--) {
        auto it = find(*i);
        if (it) {
            it->setPinned(true);
            insertTopLevelItem(0, takeTopLevelItem(indexOfTopLevelItem(it)));
        }
    }
    pinBasicItems();
}
