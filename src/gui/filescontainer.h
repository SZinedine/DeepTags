#ifndef FILESCONTAINER_H
#define FILESCONTAINER_H

#include <QListWidget>
#include <QStyledItemDelegate>
#include "../element/element.h"
#include "fileitem.h"

class QMouseEvent;

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
