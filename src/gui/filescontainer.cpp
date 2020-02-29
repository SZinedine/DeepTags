#include "filescontainer.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <cstdlib>
#include "elementdialog.h"
#include "settings.h"
#define real(item) (static_cast<FileItem*>(item))

FilesContainer::FilesContainer(QWidget* parent) : QListWidget(parent) {
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesContainer::itemDoubleClicked, this, &FilesContainer::openFile);
    connect(this, &FilesContainer::rightClick, this, &FilesContainer::showContextMenu);
    connect(this, &FilesContainer::itemSelectionChanged, this, [&] {
        FileItem* it = real(currentItem());
        QString p    = "";
        if (it) p = it->pathQstr();
        emit selectionChanged_(p);
    });
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setItemDelegate(new CustomDelegateListWidget(this));
    setIconSize(QSize(35, 20));
}

FilesContainer::~FilesContainer() {
    clearView();
}


void FilesContainer::addFiles(QVector<Element*>* items) {
    clearView();
    for (Element* i : *items) addFile(i);
    sortAndPin();
    emit numberOfElementsChanged();
}

void FilesContainer::clearView() {
    auto remaining = count();
    while (0 <= remaining) {
        delete real(takeItem(remaining));
        remaining--;
    }
    clear();
    emit numberOfElementsChanged();
}

void FilesContainer::openFile(QListWidgetItem* item) {
    if (!item) return;
    fs::path path = real(item)->path();
    Settings::openFile("", path, parentWidget());
    emit openedFile(path);
}

void FilesContainer::openFile_(QListWidgetItem* item, const QString& editor) {
    if (!item) return;
    fs::path path = real(item)->path();
    Settings::openFile(editor, path, parentWidget());
    emit openedFile(path);
}

void FilesContainer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        QPoint qp(event->pos());
        setCurrentItem(itemAt(qp));   // select when right clicked
        emit rightClick(qp);
    } else
        QListWidget::mousePressEvent(event);
}

void FilesContainer::showContextMenu(const QPoint& pos) {
    if (count() == 0) return;
    QListWidgetItem* item = itemAt(pos);
    if (!item) return;
    FileItem* real_it = real(item);

    auto menu = std::make_unique<QMenu>();
    auto pin  = std::make_unique<QAction>(tr("Pin to Top"), this);
    auto fav  = std::make_unique<QAction>(tr("Favorite"), this);
    pin->setShortcut(QKeySequence("Ctrl+p"));
    fav->setShortcut(QKeySequence("Ctrl+s"));
    pin->setCheckable(true);
    fav->setCheckable(true);
    pin->setChecked(real_it->pinned());
    fav->setChecked(real_it->favorited());

    connect(pin.get(), &QAction::triggered, this, &FilesContainer::pinSelected);
    connect(fav.get(), &QAction::triggered, this, &FilesContainer::starSelected);

    // list of editors in a sub menu "open with..."
    QStringList editor_list = Settings::mdEditors();
    auto openWith           = std::make_unique<QMenu>(tr("Open with"), this);
    for (auto& i : editor_list) {
        auto edac = new QAction(i, openWith.get());
        openWith->addAction(edac);
        connect(edac, &QAction::triggered, this, [=] { openFile_(item, i); });
    }

    menu->addAction(tr("Open"), this, [=]() { openFile(item); });
    if (editor_list.size() > 1) menu->addMenu(openWith.get());
    menu->addAction(
        tr("Edit"), this, [=]() { editElement(item); }, QKeySequence("Ctrl+e"));
    menu->addSeparator();
    menu->addAction(tr("Add a new tag"), this, [=]() { appendNewTagToItem(item); });
    menu->addAction(pin.get());
    menu->addAction(fav.get());
    menu->addSeparator();

    // if the element is deleted, that means that we are in the Trash tag
    if (real_it->element()->deleted()) {
        menu->addAction(tr("Restore"), this, &FilesContainer::restoreSelected,
                        QKeySequence("Ctrl+r"));
        menu->addAction(
            tr("Delete Permanently"), this, [=]() { permanentlyDelete(item); },
            QKeySequence(Qt::Key_Shift + Qt::Key_Delete));
    } else
        menu->addAction(tr("Move to Trash"), this, &FilesContainer::trashSelected,
                        QKeySequence(QKeySequence::Delete));

    menu->exec(mapToGlobal(pos));
}

void FilesContainer::restoreSelected() {
    if (!hasFocus()) return;
    FileItem* item = real(currentItem());
    if (!item) return;
    Element* e = item->element();
    if (!e) return;
    if (!e->deleted()) return;
    e->changeDeleted(false);
    takeItem(row(item));
    emit elementChanged(e);
}

void FilesContainer::pinSelected() {
    if (!hasFocus()) return;
    FileItem* item = real(currentItem());
    if (!item) return;
    Element* element = item->element();
    if (!element) return;
    element->changePinned(!element->pinned());
    element->reload();
    item->reload();
    emit elementChanged(element);
    sortAndPin();
}

void FilesContainer::starSelected() {
    if (!hasFocus()) return;
    FileItem* item = real(currentItem());
    if (!item) return;
    Element* element = item->element();
    if (!element) return;
    element->changeFavorited(!element->favorited());
    element->reload();
    item->reload();
    emit elementChanged(element);
    sortAndPin();
}


void FilesContainer::sortAndPin() {
    QListWidgetItem* current = currentItem();
    sortItems(Qt::AscendingOrder);

    QVector<int> pinned;
    // look for pinned elements and get their indexes into pinned:QVector
    for (int i = 0; i < count(); i++)
        if (real(item(i))->pinned()) pinned.push_back(i);

    int index = 0;
    for (const int& i : pinned) {   // take the items and pin them
        QListWidgetItem* item = takeItem(i);
        insertItem(index, item);
        index++;
    }
    if (current) setCurrentItem(current);
}

void FilesContainer::moveToTrash(QListWidgetItem* item) {
    if (!item) return;
    auto e = real(item)->element();
    if (e->deleted()) return;   // if it is deleted, it is already in Trash
    e->changeDeleted(true);
    emit elementChanged(e);
    delete takeItem(row(item));
}


void FilesContainer::permanentlyDelete(QListWidgetItem* item) {
    if (!hasFocus()) return;
    auto ok =
        QMessageBox::question(parentWidget(), tr("Permanently delete a file"),
                              tr("The file will permanently be deleted. Do you want to proceed?"));
    if (ok == QMessageBox::No) return;
    auto it  = real(item);
    bool rem = fs::remove(it->element()->path());
    if (!rem) return;
    emit deletedItem(real(item)->element());
    delete real(takeItem(row(item)));
}


void FilesContainer::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasText() && count() > 0)
        event->accept();
    else
        event->ignore();
}


void FilesContainer::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void FilesContainer::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasText()) {
        if (count() == 0) return;
        QPoint pos     = event->pos();
        FileItem* item = real(itemAt(pos));
        if (!item) return;
        setCurrentItem(item);

        const QString tag        = event->mimeData()->text();
        const QString actionText = QString(tr("Add the tag") + QString(" '") + tag + QString("'"));

        // context menu
        auto menu = std::make_unique<QMenu>();
        menu->addAction(actionText, this, [=]() { appendTagToItem(tag, item); });
        menu->exec(mapToGlobal(event->pos()));

        event->setDropAction(Qt::CopyAction);
        event->accept();

    } else
        event->ignore();
}


void FilesContainer::appendTagToItem(const QString& tag, FileItem* item) {
    if (!item) return;
    item->element()->appendTag(tag.toStdString());
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
    QString tag      = QInputDialog::getText(this, tr("Append New Tag"), lb, QLineEdit::Normal);
    if (tag.isEmpty()) return;
    appendTagToItem(tag, real(item));
    real(item)->element()->reload();
    emit elementChanged(real(item)->element());
}


void FilesContainer::editElement(QListWidgetItem* item) {
    if (!item) return;
    FileItem* it   = real(item);
    Element* e     = it->element();
    auto edit      = std::make_unique<ElementDialog>(e, this);
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
    for (int i = 0; i < count(); i++) {
        FileItem* current = real(item(i));
        if (current->path() == path) return current;
    }
    return nullptr;
}
