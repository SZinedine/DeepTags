#ifndef FILESCONTAINER_H
#define FILESCONTAINER_H

#include <QListWidget>
#include "../element/element.h"
#include <QMouseEvent>
#include "fileitem.h"
#include <QStyledItemDelegate>
class FilesContainer : public QListWidget
{
    Q_OBJECT
public:
    explicit FilesContainer(QWidget *parent=nullptr);
    ~FilesContainer() override;
    static inline FileItem* real(QListWidgetItem* item)	{	return dynamic_cast<FileItem*>(item);	}

public slots:
    void addFiles(QVector<Element*>* items);
    void openFile(QListWidgetItem* item);			// open the file in a Markdown Editor (to be set beforehand)
    void showContextMenu(const QPoint& p);
    void clearView();
    void removeItem(QListWidgetItem* item);
    void appendTagToItem(const QString& tag, FileItem* item);
    void overrideTags(const StringList& tags, FileItem* item);
    void appendNewTagToItem(QListWidgetItem* item);
    void editElement(QListWidgetItem* item);
    FileItem* itemFromPath(const fs::path& path);

private:
    void mousePressEvent(QMouseEvent *event) override;		// to capture right clicks
    void sortAndPin();										// pin the necessary files
    void addFile(Element* item);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void rightClick(QPoint pos);
    void numberOfElementsChanged();
    void removedItem(Element* item);
    void elementChanged(Element* element);
};


/**
 * subclass QStyledItemDel in order to align the item icons (favorite/pinned) to the right
 */
class CustomDelegateListWidget : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CustomDelegateListWidget(QObject *parent=nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem myOpt(option);
        myOpt.decorationPosition = QStyleOptionViewItem::Right;
        QStyledItemDelegate::paint(painter, myOpt, index);
    }
};


#endif // FILESCONTAINER_H
