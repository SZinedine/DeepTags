/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2021  Zineddine Saibi
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
#ifndef TAGSCONTAINER_H
#define TAGSCONTAINER_H

#include "tagitem.h"

class QTreeWidget;
class QWidget;
// convinient variables
static const std::vector<QString> basicTags = { QObject::tr("All Notes"), QObject::tr("Notebooks"),
                                                QObject::tr("Favorite"), QObject::tr("Untagged") };
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
     * like selected, but the retained items are only
     * those of the current item without its children's
     */
    void selectedExclusive();
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
     *  if the parent is provided, it search in its children
     *  if not, search in the topLevelItems
     */
    TagItem* find(const QString& label, TagItem* parent = nullptr);
    /* int find(const QString& label, QTreeWidget* parent); */
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
    void itemSelected(QList<Element*>* item);
    /**
     *  emited when a list of Elements are loading
     */
    void loadingFiles();
    /**
     *  emited when a list of Elements finished loading
     */
    void filesLoaded();
};

#endif   // TAGSCONTAINER_H
