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
#include "filescontainer.h"
#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QInputDialog>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QShortcut>
#include <cstdlib>
#include <memory>
#include "benchmarks.h"
#include "elementdialog.h"
#include "settings.h"
#define real(item) (static_cast<FileItem*>(item))

FilesContainer::FilesContainer(QWidget* parent) : QListWidget(parent) {
    setupSignals();
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setItemDelegate(new CustomDelegateListWidget(this));
    setIconSize(QSize(35, 20));
}

FilesContainer::~FilesContainer() {
    clearView();
}

void FilesContainer::setupSignals() {
    connect(this, &FilesContainer::itemDoubleClicked, this, &FilesContainer::openFile);
    connect(this, &FilesContainer::rightClick, this, &FilesContainer::showContextMenu);
    connect(this, &FilesContainer::itemPressed, this, [&] {
        FileItem* it = real(currentItem());
        QString p    = "";
        if (it)
            p = it->pathQstr();
        else
            return;
        if (!it->isSelected()) return;
        emit selectionChanged_(p);
    });

    auto press = new QShortcut(QKeySequence("Space"), this);
    connect(press, &QShortcut::activated, this, [&] {
        if (!hasFocus()) return;
        if (count() == 0)
            return;
        else if (selectedItems().empty())
            setCurrentItem(item(0));
        if (!currentItem()) return;
        emit itemPressed(currentItem());
    });
}


void FilesContainer::addFiles(QList<Element*>* items) {
    clearView();
    BEGIN_TIME;
    for (auto& i : *items) addFile(i);
    sortAndPin();
    emit numberOfElementsChanged();
    END_TIME("FilesContainer::addFiles");
    SIZEOFCONTAINER("FileItems displayed (from FilesContainer::addFiles)", items->size());
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
    QString path = real(item)->path();
    Settings::openFile("", path, parentWidget());
    emit openedFile(path);
}

void FilesContainer::openFile_(QListWidgetItem* item, const QString& editor) {
    if (!item) return;
    QString path = real(item)->path();
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

    auto menu     = std::make_unique<QMenu>();
    auto open     = std::make_unique<QAction>(tr("Open"), menu.get());
    auto edit     = std::make_unique<QAction>(tr("Edit"), menu.get());
    auto newTag   = std::make_unique<QAction>(tr("Add a new tag"), menu.get());
    auto pin      = std::make_unique<QAction>(tr("Pin to Top"), menu.get());
    auto fav      = std::make_unique<QAction>(tr("Favorite"), menu.get());
    auto copyPath = std::make_unique<QAction>(tr("Copy path"), menu.get());
    auto mvtrash  = std::make_unique<QAction>(tr("Move to Trash"), menu.get());
    auto restore  = std::make_unique<QAction>(tr("Restore"), menu.get());
    auto delperm  = std::make_unique<QAction>(tr("Delete Permanently"), menu.get());

    // list of editors in a sub menu "open with..."
    QStringList editor_list = Settings::mdEditors();
    auto openWith           = std::make_unique<QMenu>(tr("Open with"), menu.get());
    for (auto& i : editor_list) {
        auto edac = new QAction(i, openWith.get());
        openWith->addAction(edac);
        connect(edac, &QAction::triggered, this, [=] { openFile_(item, i); });
    }

    open->setToolTip(Settings::mainMdEditor());
    pin->setShortcut(QKeySequence("Ctrl+p"));
    fav->setShortcut(QKeySequence("Ctrl+s"));
    edit->setShortcut(QKeySequence("Ctrl+e"));
    restore->setShortcut(QKeySequence("Ctrl+r"));
    pin->setCheckable(true);
    fav->setCheckable(true);
    pin->setChecked(real_it->pinned());
    fav->setChecked(real_it->favorited());
    delperm->setShortcut(QKeySequence(Qt::Key_Shift + Qt::Key_Delete));
    mvtrash->setShortcut(QKeySequence(QKeySequence::Delete));

    connect(open.get(), &QAction::triggered, [=] { openFile(item); });
    connect(edit.get(), &QAction::triggered, [=] { editElement(item); });
    connect(newTag.get(), &QAction::triggered, [=] { appendNewTagToItem(item); });
    connect(pin.get(), &QAction::triggered, this, &FilesContainer::pinSelected);
    connect(fav.get(), &QAction::triggered, this, &FilesContainer::starSelected);
    connect(copyPath.get(), &QAction::triggered, this,
            [=] { QApplication::clipboard()->setText(real_it->pathQstr()); });
    connect(restore.get(), &QAction::triggered, [=] { restoreSelected(); });
    connect(delperm.get(), &QAction::triggered, [=] { permanentlyDelete(item); });
    connect(mvtrash.get(), &QAction::triggered, [=] { trashSelected(); });

    // we are in the trash TagItem if the element is deleted
    if (real_it->deleted()) {
        menu->addAction(open.get());
        menu->addAction(copyPath.get());
        menu->addSeparator();
        menu->addAction(restore.get());
        menu->addAction(delperm.get());
    } else {   // all other TagItems
        menu->addAction(open.get());
        if (editor_list.size() > 1) menu->addMenu(openWith.get());
        menu->addAction(edit.get());
        menu->addSeparator();
        menu->addAction(newTag.get());
        menu->addAction(pin.get());
        menu->addAction(fav.get());
        menu->addSeparator();
        menu->addAction(copyPath.get());
        menu->addAction(mvtrash.get());
    }
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
    bool rem = QFile::remove(it->element()->path());
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
    if (event->mimeData()->hasText() && !event->mimeData()->text().isEmpty()) {
        if (count() == 0) return;
        QPoint pos     = event->pos();
        FileItem* item = real(itemAt(pos));
        if (!item) return;
        setCurrentItem(item);

        const QString tag        = event->mimeData()->text();
        const QString actionText = QString(tr("Add the tag") + QString(" '") + tag + QString("'"));

        // context menu
        auto menu          = std::make_unique<QMenu>();
        auto addTagsAction = menu->addAction(actionText);
        connect(addTagsAction, &QAction::triggered, [=] { appendTagToItem(tag, item); });

        menu->exec(mapToGlobal(event->pos()));

        event->setDropAction(Qt::CopyAction);
        event->accept();

    } else
        event->ignore();
}


void FilesContainer::appendTagToItem(const QString& tag, FileItem* item) {
    if (!item) return;
    item->element()->appendTag(tag);
    emit elementChanged(item->element());
    item->setText(item->element()->title());
}


void FilesContainer::overrideTags(const StringList& tags, FileItem* item) {
    if (!item) return;
    item->element()->overrideTags(tags);
    item->setText(item->element()->title());
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
    FileItem* it = real(item);
    Element* e   = it->element();
    auto edit    = std::make_unique<ElementDialog>(e, this);
    if (edit->exec() == ElementDialog::Rejected) return;

    e->changeTitle(edit->title());
    e->changePinned(edit->pinned());
    e->changeFavorited(edit->favorited());
    it->reload();
    overrideTags(edit->tags(), it);
    sortAndPin();
}


FileItem* FilesContainer::itemFromPath(const QString& path) {
    for (int i = 0; i < count(); i++) {
        FileItem* current = real(item(i));
        if (current->path() == path) return current;
    }
    return nullptr;
}
