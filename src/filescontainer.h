/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2020  Zineddine Saibi
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
#ifndef FILESCONTAINER_H
#define FILESCONTAINER_H

#include <QStyledItemDelegate>
#include "element.h"
#include "fileitem.h"

class QMouseEvent;
class QListWidget;

class FilesContainer : public QListWidget {
    Q_OBJECT
public:
    explicit FilesContainer(QWidget* parent = nullptr);
    ~FilesContainer() override;
    void setupSignals();
    void addFiles(QVector<Element*>* items);
    /**
     *  open the file in a Markdown Editor (to be set beforehand)
     */
    void openFile(QListWidgetItem* item);
    void openFile_(QListWidgetItem* item, const QString& editor);
    inline void openCurrent() { openFile(currentItem()); }
    void showContextMenu(const QPoint& p);
    void clearView();
    void moveToTrash(QListWidgetItem* item);
    void permanentlyDelete(QListWidgetItem* item);
    void appendTagToItem(const QString& tag, FileItem* item);
    void overrideTags(const StringList& tags, FileItem* item);
    void appendNewTagToItem(QListWidgetItem* item);
    void editElement(QListWidgetItem* item);
    FileItem* itemFromPath(const QString& path);

public slots:
    void pinSelected();
    void starSelected();
    inline void trashSelected() { moveToTrash(currentItem()); }
    inline void permanentlyDeleteSelected() { permanentlyDelete(currentItem()); }
    void restoreSelected();

private:
    /**
     * insert an item into the view
     */
    inline void addFile(Element* item) { addItem(new FileItem(item, this)); }
    /**
     *  pin the necessary files at the top
     */
    void sortAndPin();
    /**
     *  to capture right clicks
     */
    void mousePressEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void rightClick(QPoint pos);
    void numberOfElementsChanged();
    void deletedItem(Element* item);
    void restoredElement(Element* e);
    void elementChanged(Element* element);
    void openedFile(QString f);   // open in a markdown editor
    void selectionChanged_(QString f);
};


/**
 * subclass QStyledItemDel in order to align the item icons (favorite/pinned) to the right
 */
class CustomDelegateListWidget : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CustomDelegateListWidget(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        QStyleOptionViewItem myOpt(option);
        myOpt.decorationPosition = QStyleOptionViewItem::Right;
        QStyledItemDelegate::paint(painter, myOpt, index);
    }
};

#endif   // FILESCONTAINER_H
