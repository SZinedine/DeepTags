#include "filescontainer.h"
#include "fileitem.h"
#include <QDebug>
#include <cstdlib>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <thread>

FilesContainer::FilesContainer(QWidget *parent)
    : QListWidget(parent)
{
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesContainer::itemDoubleClicked, this, &FilesContainer::openFile);
    connect(this, &FilesContainer::rightClick, this, &FilesContainer::showContextMenu);
}


void FilesContainer::addFile(Element* item) {
    FileItem *f = new FileItem(item);
    addItem(f);
    sortAndPin();
}

void FilesContainer::addFiles(QVector<Element*>* items) {
    clearView();
    for (Element* i : *items)
        addFile(i);
    emit numberOfElementsChanged();
}

void FilesContainer::clearView() {
    clear();
    emit numberOfElementsChanged();
}

void FilesContainer::openFile(QListWidgetItem* item) {
    // retrieve the name of the Markdown reader
    QSettings s;
    s.beginGroup("Main");
    QString prog = s.value("MarkDownReader").toString().simplified();
    s.endGroup();

    if (prog.isEmpty()) {	// warning and abort if the reader isn't set
        QMessageBox::warning(parentWidget(), "Error",
                             "You haven't set the Markdown Editor app.");
        return;
    }

    prog.append(" ");	// space between the command name and the filepath
    fs::path p = real(item)->path();
    std::string f = '"' + p.string() + '"';

    std::string command = prog.toStdString() + f;
    std::thread( [=]{	std::system(command.c_str());	} )
            .detach();
}


void FilesContainer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint qp(event->pos());
        emit rightClick(qp);
    }
    else QListWidget::mousePressEvent(event);
}

void FilesContainer::showContextMenu(const QPoint& pos) {
    QPoint globalPos = mapToGlobal(pos);

    QMenu* menu = new QMenu;
    menu->addAction("Open",   this, 	[=](){ 	openFile(itemAt(pos)); 		});
    menu->addAction("Remove", this,		[=](){	removeItem(itemAt(pos)); 	});
//    menu->addAction("Remove", this,		[=](){	std::thread( [=](){removeItem(itemAt(pos));} ).detach();	}   );

    menu->exec(globalPos);
}


void FilesContainer::sortAndPin() {
    sortItems(Qt::AscendingOrder);

    QVector<int> pinned;
    for (int i = 0 ; i < count() ; i++)		// look for pinned elements and get their indexes into pinned:QVector
        if ( real(item(i))->pinned() )
            pinned.push_back(i);

    int index = 0;
    for(const int& i : pinned) {		// take the items and pin them
        QListWidgetItem* item = takeItem(i);
        insertItem(index, item);
        index++;
    }
}

void FilesContainer::removeItem(QListWidgetItem* item) {
    emit removedItem(real(item)->element());
    delete takeItem(row(item));
}

