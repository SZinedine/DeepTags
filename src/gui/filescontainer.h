#ifndef FILESCONTAINER_H
#define FILESCONTAINER_H

#include <QListWidget>
#include "../back/element.h"
#include <QMouseEvent>
#include "fileitem.h"

class FilesContainer : public QListWidget
{
    Q_OBJECT
public:
    explicit FilesContainer(QWidget *parent=nullptr);

    static inline FileItem* real(QListWidgetItem* item)	{	return dynamic_cast<FileItem*>(item);	}

public slots:
    void addFiles(QVector<Element*>* items);
    void addFile(Element* item);
    void openFile(QListWidgetItem* item);
    void showContextMenu(const QPoint& p);
    void clearView();
    void removeItem(QListWidgetItem* item);

private:
    void mousePressEvent(QMouseEvent *event);		// to capture right clicks
    void sortAndPin();								// pin the necessary files

signals:
    void rightClick(QPoint pos);
    void numberOfElementsChanged();
    void removedItem(Element* item);
};

#endif // FILESCONTAINER_H
