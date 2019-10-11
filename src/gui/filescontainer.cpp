#include "filescontainer.h"
#include "fileitem.h"
#include <QDebug>
#include <cstdlib>
#include <QMessageBox>
#include <QMenu>
#include <thread>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QInputDialog>
#include "elementdialog.h"
#include "settings.h"

FilesContainer::FilesContainer(QWidget *parent)
    : QListWidget(parent)
{
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesContainer::itemDoubleClicked, this, &FilesContainer::openFile);
    connect(this, &FilesContainer::rightClick, this, &FilesContainer::showContextMenu);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setItemDelegate(new CustomDelegateListWidget(this));
    setIconSize(QSize(35, 20));
}

FilesContainer::~FilesContainer() { clear(); }

void FilesContainer::addFile(Element* item) {
    auto *f = new FileItem(item);
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
    if (!item) return;
    fs::path path = real(item)->path();
    Settings::openFile_(path, parentWidget());
    emit openedFile(path);
}


void FilesContainer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPoint qp(event->pos());
        setCurrentItem( itemAt(qp) );        // select when right clicked
        emit rightClick(qp);
    }
    else QListWidget::mousePressEvent(event);
}

void FilesContainer::showContextMenu(const QPoint& pos) {
    if (count() == 0) return;
    QPoint globalPos = mapToGlobal(pos);
    QListWidgetItem* item = itemAt(pos);
    if (!item) return;
    FileItem* real_it = real(item);

    auto abstraction = [=](){
        real_it->element()->reload();
        real_it->reload();
        emit elementChanged(real_it->element());
        sortAndPin();
    };

    QAction* pin = new QAction(tr("Pin to Top"));
    QAction* fav = new QAction(tr("Favorite"));
    pin->setCheckable(true);
    fav->setCheckable(true);
    pin->setChecked(real_it->pinned());
    fav->setChecked(real_it->favorited());

    connect(pin, &QAction::triggered, this, [=](){
        real_it->element()->changePinned(pin->isChecked());
        abstraction();
    });
    connect(fav, &QAction::triggered, this, [=](){
        real_it->element()->changeFavorited(fav->isChecked());
        abstraction();
    });

    QMenu* menu = new QMenu;
    menu->addAction(tr("Open"),         this,   [=](){  openFile(item);           });
    menu->addAction(tr("Edit"),         this,   [=](){  editElement(item);        });
    menu->addSeparator();
    menu->addAction(tr("Add a new tag"),this,   [=](){  appendNewTagToItem(item); });
    menu->addAction(pin);
    menu->addAction(fav);
    menu->addSeparator();

    // if the element is deleted, that means that we are in the Trash tag
    if (real_it->element()->deleted()) {
        menu->addAction(tr("Restore"),  this,   [=](){
            (real_it->element())->changeDeleted(false);
           takeItem(row(item));
           emit elementChanged(real_it->element());
        });
        menu->addAction(tr("Delete Permanently"),  this,   [=](){  permanentlyDelete(item);         });
    }
    else
        menu->addAction(tr("Move to Trash"),   this,   [=](){  moveToTrash(item);         });

    menu->exec(globalPos);

    delete menu;
    delete pin;
    delete fav;
}


void FilesContainer::sortAndPin() {
    QListWidgetItem* current = currentItem();
    sortItems(Qt::AscendingOrder);

    QVector<int> pinned;
    // look for pinned elements and get their indexes into pinned:QVector
    for (int i = 0 ; i < count() ; i++)
        if ( real(item(i))->pinned() )
            pinned.push_back(i);

    int index = 0;
    for(const int& i : pinned) {        // take the items and pin them
        QListWidgetItem* item = takeItem(i);
        insertItem(index, item);
        index++;
    }
    if (current) setCurrentItem(current);
}

void FilesContainer::moveToTrash(QListWidgetItem* item) {
    if (!item) return;
    auto e = real(item)->element();
    e->changeDeleted(true);
    emit elementChanged(e);
    delete takeItem(row(item));
}


void FilesContainer::permanentlyDelete(QListWidgetItem* item) {
    auto ok = QMessageBox::question(parentWidget(), tr("Permanently delete a file"),
            tr("The file will permanently be deleted. Do you want to proceed?"));
    if (ok == QMessageBox::No) return;
    auto it = real(item);
    bool rem = fs::remove(it->element()->path());
    if (!rem) {
        QMessageBox::critical(parentWidget(), tr("Deletion failed"),
                tr("Error. Failed to delete the file"));
        return;
    }
    emit deletedItem(real(item)->element());
    delete real(takeItem(row(item)));
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
        setCurrentItem(item);

        const QString tag = event->mimeData()->text();
        const QString actionText = QString(tr("Add the tag") + QString(" '") + tag + QString("'"));

        // context menu
        QMenu* menu = new QMenu;
        menu->addAction(actionText, this, [=](){    appendTagToItem(tag, item);     });
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
    const QString lb = tr("Write the new Tag to append");
    QString tag = QInputDialog::getText(this, tr("Append New Tag"),
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
    auto *edit = new ElementDialog(e, this);
    const auto out = edit->exec();
    if (out == ElementDialog::Rejected) return;

    e->changeTitle(edit->title());
    e->changePinned(edit->pinned());
    e->changeFavorited(edit->favorited());
    it->reload();
    overrideTags(edit->tags(), it);
    sortAndPin();
}




FileItem* FilesContainer::itemFromPath(const fs::path& path) {
    for (int i = 0 ; i < count() ; i++) {
        FileItem* current = real(item(i));
        if (current->path() == path) return current;
    }
    return nullptr;
}







