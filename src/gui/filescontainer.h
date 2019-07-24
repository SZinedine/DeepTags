#ifndef FILESCONTAINER_H
#define FILESCONTAINER_H

#include <QListWidget>
#include "../element/element.h"
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
    void openFile(QListWidgetItem* item);			// open the file in a Markdown Editor (to be set beforehand)
    void showContextMenu(const QPoint& p);
    void clearView();
    void removeItem(QListWidgetItem* item);

private:
    void mousePressEvent(QMouseEvent *event);		// to capture right clicks
    void sortAndPin();								// pin the necessary files
    void addFile(Element* item);

signals:
    void rightClick(QPoint pos);
    void numberOfElementsChanged();
    void removedItem(Element* item);
};

#endif // FILESCONTAINER_H
