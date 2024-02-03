#include "DocumentsListView.h"
#include <Settings.h>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QShortcut>
#include <memory>
#include "Document.h"
#include "DocumentInfoDialog.h"
#include "DocumentListDelegate.h"
#include "DocumentListModel.h"

using Ui::Models::DocumentListModel;
using Ui::Delegates::DocumentListDelegate;
using std::make_unique;
namespace Settings = Ui::Settings;

const auto ctrl_p  = QKeySequence::fromString(QStringLiteral("Ctrl+p"));
const auto ctrl_s  = QKeySequence::fromString(QStringLiteral("Ctrl+s"));
const auto ctrl_e  = QKeySequence::fromString(QStringLiteral("Ctrl+e"));
const auto return_ = QKeySequence::fromString(QStringLiteral("Return"));
const auto space_  = QKeySequence::fromString(QStringLiteral("Space"));
constexpr QSize iconSize_(35, 20);

DocumentsListView::DocumentsListView(QWidget* parent)
    : QListView(parent), mDeletegate{ new DocumentListDelegate(this) } {
    setItemDelegate(mDeletegate);
    setDragDropMode(DropOnly);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setIconSize(iconSize_);
    setUniformItemSizes(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    // setAlternatingRowColors(true);

    connect(this, &DocumentsListView::clicked, this, &DocumentsListView::onDocumentSelected);
    connect(this, &DocumentsListView::doubleClicked, this, &DocumentsListView::openDocumentAt);
    connect(this, &DocumentsListView::customContextMenuRequested, this,
            &DocumentsListView::onCustomContextMenuRequested);

    auto openDoc_      = [this] { openDocumentAt(currentIndex()); };
    auto toggleBoolPin = [this] { toggleBool(currentIndex(), DocumentListModel::PinnedRole); };
    auto toggleBoolFav = [this] { toggleBool(currentIndex(), DocumentListModel::FavoritedRole); };
    auto launchDID     = [this] { launchDocumentInfoDialog(currentIndex()); };
    auto docSelected   = [this] { onDocumentSelected(currentIndex()); };
    mShortcuts.push_back(make_unique<QShortcut>(return_, this, openDoc_));
    mShortcuts.push_back(make_unique<QShortcut>(ctrl_p, this, toggleBoolPin));
    mShortcuts.push_back(make_unique<QShortcut>(ctrl_s, this, toggleBoolFav));
    mShortcuts.push_back(make_unique<QShortcut>(ctrl_e, this, launchDID));
    mShortcuts.push_back(make_unique<QShortcut>(space_, this, docSelected));
}


DocumentsListView::~DocumentsListView() {
    delete mDeletegate;
}


void DocumentsListView::onCustomContextMenuRequested(const QPoint& pos) {
    const auto index = indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    Doc::Document* const doc = model()->getDocumentAt(index.row());

    if (doc == nullptr) {
        return;
    }

    auto menu = std::make_unique<QMenu>();

    menu->addAction(tr("Open"), this, [this, doc] {
        emit documentToOpen(doc->getPath(), Ui::Settings::loadDefaultExternalReaders());
    });

    if (const auto readers = Settings::loadExternalReaders(); !readers.isEmpty()) {
        auto openWith = menu->addMenu(tr("Open With"));
        for (const auto& editor : Settings::loadExternalReaders()) {
            openWith->addAction(
                editor, this, [this, editor, doc] { emit documentToOpen(doc->getPath(), editor); });
        }
    }

    if (!doc->isDeleted()) {
        menu->addSeparator();

        auto editAction =
            menu->addAction(tr("Edit"), this, [this, index] { launchDocumentInfoDialog(index); });

        menu->addAction(tr("Add a new tag"), this, [this, index] { addTagDialog(index); });

        const QString pinStr = doc->isPinned() ? tr("Unpin") : tr("Pin To Top");
        auto pinAction       = menu->addAction(
            pinStr, this, [this, index] { toggleBool(index, DocumentListModel::PinnedRole); });
        auto favAction = menu->addAction(tr("Favorite"), this, [this, index] {
            toggleBool(index, DocumentListModel::FavoritedRole);
        });

        editAction->setShortcut(ctrl_e);
        pinAction->setShortcut(ctrl_p);
        favAction->setShortcut(ctrl_s);
        pinAction->setCheckable(true);
        favAction->setCheckable(true);
        pinAction->setChecked(doc->isPinned());
        favAction->setChecked(doc->isFavorited());
    }

    menu->addSeparator();
    menu->addAction(tr("Copy path"), this, [this, index] { model()->copyPath(index); });

    if (doc->isDeleted()) {
        menu->addAction(tr("Restore"), this,
                        [this, index] { model()->setData(index, false, model()->DeletedRole); });
        menu->addAction(tr("Delete Permanently"), this,
                        [this, index] { permanentlyDelete(index); });
    } else {
        menu->addAction(tr("Move to Trash"), this,
                        [this, index] { model()->setData(index, true, model()->DeletedRole); });
    }

    menu->exec(mapToGlobal(pos));
}


void DocumentsListView::onDocumentSelected(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    if (auto doc = model()->getDocumentAt(index)) {
        emit documentSelected(doc);
    }
}


void DocumentsListView::addTagDialog(const QModelIndex& index) {
    const QString title = tr("Append New Tag");
    const QString label = tr("Write the new Tag to append");
    auto tag            = QInputDialog::getText(this, title, label, QLineEdit::Normal);

    Doc::Utils::sanitizeTag(tag);
    if (Doc::Utils::isValidTag(tag)) {
        model()->setData(index, tag, model()->TagAddRole);
    }
}


void DocumentsListView::openDocumentAt(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    auto path = model()->data(index, model()->PathRole).toString();
    emit documentToOpen(path, Settings::loadDefaultExternalReaders());
}


void DocumentsListView::permanentlyDelete(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    auto doc = model()->getDocumentAt(index);
    auto res = QMessageBox::question(
        this, tr("Delete Permanently"),
        tr("Do you want to permanently delete \"<b>%1</b>\"").arg(doc->getTitle()));

    if (res == QMessageBox::Yes) {
        model()->setData(index, {}, model()->PermanentlyDeletedRole);
    }
}


void DocumentsListView::launchDocumentInfoDialog(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    auto document = model()->getDocumentAt(index);
    auto dialog   = std::make_unique<DocumentInfoDialog>(document, nullptr);
    if (dialog->exec() == dialog->Rejected) {
        return;
    }

    if (bool favorited = dialog->isFavorited(); document->isFavorited() != favorited) {
        model()->setData(document, favorited, Ui::Models::DocumentListModel::FavoritedRole);
    }

    if (bool pinned = dialog->isPinned(); document->isPinned() != pinned) {
        model()->setData(document, pinned, Ui::Models::DocumentListModel::PinnedRole);
    }

    if (auto title = dialog->getTitle(); document->getTitle() != title) {
        model()->setData(document, title, Ui::Models::DocumentListModel::TitleRole);
    }

    if (auto tags = dialog->getTags(); document->getTags() != tags) {
        // check if any deleted tags
        for (QString tag : document->getTags()) {
            Doc::Utils::sanitizeTag(tag);
            if (!Doc::Utils::isValidTag(tag)) {
                continue;
            }

            if (!dialog->getTags().contains(tag)) {
                model()->setData(document, tag, Ui::Models::DocumentListModel::TagDeleteRole);
            }
        }

        // check for new tags and add them
        for (QString tag : tags) {
            Doc::Utils::sanitizeTag(tag);
            if (!Doc::Utils::isValidTag(tag)) {
                continue;
            }

            if (!document->containsExactTag(tag)) {
                model()->setData(document, tag, Ui::Models::DocumentListModel::TagAddRole);
            }
        }
    }

    setCurrentDocument(document);
}


void DocumentsListView::dropEvent(QDropEvent* event) {
    const auto data = event->mimeData();
    if (data->hasText() && !data->text().isEmpty()) {
        const QString tag  = data->text();
        auto index         = indexAt(event->pos());
        Doc::Document* doc = model()->getDocumentAt(index);

        if (doc == nullptr) {
            return;
        }

        auto actionText = tr("Add the tag '%1'").arg(data->text());

        // context menu
        auto menu          = std::make_unique<QMenu>();
        auto addTagsAction = menu->addAction(actionText);
        connect(addTagsAction, &QAction::triggered, this,
                [=] { model()->setData(index, tag, model()->TagsRole); });

        menu->exec(mapToGlobal(event->pos()));

        event->setDropAction(Qt::CopyAction);
        event->accept();

    } else {
        event->ignore();
    }
}


void DocumentsListView::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void DocumentsListView::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasText()) {
        event->accept();
    } else {
        event->ignore();
    }
}


DocumentListModel* DocumentsListView::model() {
    return static_cast<DocumentListModel*>(QListView::model());
}


void DocumentsListView::setCurrentDocument(Doc::Document* document) {
    if (auto modelIndex = model()->index(document); modelIndex.isValid()) {
        setCurrentIndex(modelIndex);
    }
}


void DocumentsListView::toggleBool(const QModelIndex& index, int role) {
    if (!index.isValid()) {
        return;
    }

    bool value    = false;
    auto document = model()->getDocumentAt(index.row());

    switch (role) {
    case DocumentListModel::PinnedRole:
        value = !model()->data(index, DocumentListModel::PinnedRole).toBool();
        break;
    case DocumentListModel::FavoritedRole:
        value = !model()->data(index, DocumentListModel::FavoritedRole).toBool();
        break;
    case DocumentListModel::DeletedRole:
        value = !model()->data(index, DocumentListModel::DeletedRole).toBool();
        break;
    default:
        return;
    }

    model()->setData(index, value, role);

    setCurrentDocument(document);
}
