#include "filescontainer.h"
#include "fileitem.h"
#include <QDebug>
#include <cstdlib>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <thread>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QInputDialog>
#include "elementdialog.h"

FilesContainer::FilesContainer(QWidget *parent)
    : QListWidget(parent)
{
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesContainer::itemDoubleClicked, this, &FilesContainer::openFile);
    connect(this, &FilesContainer::rightClick, this, &FilesContainer::showContextMenu);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

FilesContainer::~FilesContainer() { clear(); }

void FilesContainer::addFile(Element* item) {
    FileItem *f = new FileItem(item);
    addItem(f);
}

void FilesContainer::addFiles(QVector<Element*>* items) {
    clearView();
    for (Element* i : *items)
        addFile(i);
    sortAndPin();
    emit numberOfElementsChanged();
}

void FilesContainer::clearView() {
    clear();
    emit numberOfElementsChanged();
}

void FilesContainer::openFile(QListWidgetItem* item) {
    // retrieve the name of the Markdown reader
    if (!item) return;
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
    if (count() == 0) return;
    QPoint globalPos = mapToGlobal(pos);

    QMenu* menu = new QMenu;
    menu->addAction("Open",   this, 	[=](){ 	openFile(itemAt(pos)); 		});
    menu->addAction("Remove", this,		[=](){	removeItem(itemAt(pos)); 	});
    menu->addAction("edit", this,		[=](){	editElement(itemAt(pos)); 	});
    menu->addAction("Add a new tag", this, [=](){	appendNewTagToItem(itemAt(pos));	});
    menu->exec(globalPos);
    delete menu;
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
    if (!item) return;
    emit removedItem(real(item)->element());
    delete takeItem(row(item));
}



void FilesContainer::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() && count() > 0)
        event->accept();
    else
        event->ignore();
}


void FilesContainer::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void FilesContainer::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        if (count() == 0) return;
        QPoint pos = event->pos();
        FileItem* item = real(itemAt(pos));
        if (!item) return;

        const QString tag = event->mimeData()->text();
        const QString actionText = QString("Add the tag '") + tag + QString("'");

        // context menu
        QMenu* menu = new QMenu;
        menu->addAction(actionText, this, [=](){	appendTagToItem(tag, item);		});
        menu->exec(mapToGlobal( event->pos()) );

        delete menu;

        event->setDropAction(Qt::CopyAction);
        event->accept();

    } else
        event->ignore();
}


void FilesContainer::appendTagToItem(const QString& tag, FileItem* item) {
    if (!item) return;
    item->element()->appendTag( tag.toStdString() );
    emit elementChanged(item->element());
    item->setText(QString::fromStdString(item->element()->title()));
}


void FilesContainer::overrideTags(const StringList& tags, FileItem* item) {
    if (!item) return;
    item->element()->overrideTags(tags);
    item->setText(QString::fromStdString(item->element()->title()));
    item->element()->reload();
    emit elementChanged(item->element());
}


void FilesContainer::appendNewTagToItem(QListWidgetItem* item) {
    const QString lb = "Write the new Tag to append";
    QString tag = QInputDialog::getText(this, "Append New Tag",
                                         lb,
                                         QLineEdit::Normal);
    if (tag.isEmpty()) return;
    appendTagToItem(tag, real(item));
    real(item)->element()->reload();
    emit elementChanged(real(item)->element());
}



void FilesContainer::editElement(QListWidgetItem* item) {
    if (!item) return;
    FileItem* it = real(item);
    Element* e = it->element();
    ElementDialog* edit = new ElementDialog(e, this);
    const auto out = edit->exec();
    if (out == ElementDialog::Rejected) return;

    e->changeTitle(edit->title());
    e->changePinned(edit->pinned());
    e->changeFavorited(edit->favorited());
    it->setLabel(e->title());
    overrideTags(edit->tags(), it);
}






















