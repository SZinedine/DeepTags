#include "DocumentListModel.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <algorithm>
#include "Benchmark.h"
#include "Document.h"
#include "DocumentListItem.h"

#define castItem(x) static_cast<DocumentListItem*>(x)
using Doc::Document;

namespace Ui::Models {

DocumentListModel::DocumentListModel(QObject* parent) : QAbstractListModel{ parent } {
    connect(this, &DocumentListModel::tagAdded, this,
            [&] { emit numberOfDocumentsChanged(mDocuments.size()); });
    connect(this, &DocumentListModel::documentPermanentlyDeleted, this,
            [&] { emit numberOfDocumentsChanged(mDocuments.size()); });
}


int DocumentListModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) {
        return mDocuments.size();
    }

    return 0;
}


QVariant DocumentListModel::data(const QModelIndex& index, int role) const {
    if (mDocuments.empty() || !index.isValid()) {
        return {};
    }

    const Document* const doc = mDocuments.at(index.row());

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
    case TitleRole:
        return doc->getTitle();
    case PathRole:
        return doc->getPath();
    case PinnedRole:
        return doc->isPinned();
    case FavoritedRole:
        return doc->isFavorited();
    case DeletedRole:
        return doc->isDeleted();
    case TagsRole:
    case TagAddRole:
    case TagDeleteRole:
        return { doc->getTags().toList() };
    case Qt::ToolTipRole: {
        const QString title = doc->getTitle();
        const QString path  = doc->getPath();
        return QStringLiteral("<p><b>%1:</b> %2</p><p><b>%3:</path> %4</p>")
            .arg(tr("Title"), title, tr("Path"), path);
    }
    }

    return {};
}


bool DocumentListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }

    bool changed = false;

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
    case TitleRole: {
        changed = Doc::Utils::setDocumentTitle(getDocumentAt(index), value.toString());
        break;
    }
    case PinnedRole:
        changed = Doc::Utils::setDocumentPinned(getDocumentAt(index), value.toBool());
        break;
    case FavoritedRole:
        changed = Doc::Utils::setDocumentFavorited(getDocumentAt(index), value.toBool());
        break;
    case DeletedRole:
        changed = Doc::Utils::setDocumentDeleted(getDocumentAt(index), value.toBool());
        break;
    case PermanentlyDeletedRole:
        if (!data(index, DeletedRole).toBool()) {
            qCritical("Error. Cannot permanently delete a file if it is not marked as 'deleted'");
        } else {
            changed = Doc::Utils::permanentlyDeleteFile(getDocumentAt(index)->getPath());
        }
        break;
    case TagsRole:
    case TagAddRole: {
        auto doc = getDocumentAt(index);
        changed  = Doc::Utils::addDocumentTag(doc, value.toString());
        if (changed && !doc->isDeleted()) {
            emit tagAdded(value.toString());
        }
        break;
    }
    case TagDeleteRole:
        changed = Doc::Utils::delDocumentTag(getDocumentAt(index), value.toString());
        if (changed) {
            emit tagDeleted(value.toString());
        }
        break;
    }

    if (changed) {
        emit dataChanged(index, index, { role });
        onDocumentChanged(index, role);
        emit numberOfDocumentsChanged(mDocuments.size());
    }

    return changed;
}


bool DocumentListModel::setData(Document* document, const QVariant& value, int role) {
    if (document == nullptr) {
        return false;
    }

    if (auto ind = index(document); ind.isValid()) {
        return setData(ind, value, role);
    }

    return false;
}


void DocumentListModel::onDocumentChanged(const QModelIndex& index, int role) {
    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
    case TitleRole:
    case PinnedRole:
        sort();
        break;
    case FavoritedRole:
        break;
    case DeletedRole: {
        auto document = getDocumentAt(index);
        if (document->isDeleted()) {   // remove tags
            for (const QString& tag : document->getTags()) {
                emit tagDeleted(tag);
            }
        } else {
            for (const QString& tag : document->getTags()) {
                emit tagAdded(tag);
            }
        }

        removeRow(index.row());
        emit documentDeleted(document);
        // sort();
        break;
    };
    case PermanentlyDeletedRole: {
        auto doc = getDocumentAt(index);
        removeRow(index.row());
        emit documentPermanentlyDeleted(doc);
        // sort();
        break;
    };
    case TagDeleteRole: {
        if (mActiveTag.isEmpty()) {
            break;
        }

        const auto doc = getDocumentAt(index);
        if (doc == nullptr) {
            break;
        }

        if (!doc->containsAllTags(mActiveTag)) {
            removeRow(index.row());
            sort();
        }

        break;
    };
    }
}


void DocumentListModel::sort([[maybe_unused]] int column, [[maybe_unused]] Qt::SortOrder order) {
    emit layoutAboutToBeChanged();
    std::sort(mDocuments.begin(), mDocuments.end(), [](auto a, auto b) { return *a < *b; });
    updatePersistentIndexes();
    emit layoutChanged();
}


Document* DocumentListModel::getDocumentAt(int row) {
    if (row < mDocuments.size()) {
        return mDocuments.at(row);
    }

    return nullptr;
}


Document* DocumentListModel::getDocumentAt(const QModelIndex& index) {
    if (!index.isValid()) {
        return nullptr;
    }

    return getDocumentAt(index.row());
}


void DocumentListModel::reset() {
    beginResetModel();
    mDocuments.clear();
    endResetModel();
    mActiveTag.clear();
    emit numberOfDocumentsChanged(mDocuments.size());
}


void DocumentListModel::setDocuments(const QVector<Document*>& documents,
                                     const QStringList& activeTag) {
    Benchmark b("document list");
    reset();
    beginInsertRows(QModelIndex(), 0, documents.size());
    mDocuments = documents;
    mActiveTag = activeTag;
    sort();
    endInsertRows();
    emit numberOfDocumentsChanged(mDocuments.size());
}


void DocumentListModel::addDocument(Document* document) {
    if (mDocuments.contains(document)) {
        return;
    }

    beginInsertRows(QModelIndex(), mDocuments.size(), mDocuments.size());
    mDocuments.push_back(document);
    sort();
    endInsertRows();
    emit numberOfDocumentsChanged(mDocuments.size());
}


[[deprecated("not used anywhere")]] void DocumentListModel::addDocuments(
    const QVector<Document*>& documents) {
    beginInsertRows(QModelIndex(), mDocuments.size(), mDocuments.size());

    for (const auto& doc : documents) {
        mDocuments.push_back(doc);
    }

    endInsertRows();
    emit numberOfDocumentsChanged(mDocuments.size());
}


void DocumentListModel::removeRow(int row) {
    if (mDocuments.empty() || row < 0 || mDocuments.size() <= row) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    mDocuments.removeAt(row);
    endRemoveRows();

    emit numberOfDocumentsChanged(mDocuments.size());
}


Qt::ItemFlags DocumentListModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}


void DocumentListModel::updatePersistentIndexes() {
    for (auto oldIndex : persistentIndexList()) {
        if (oldIndex.isValid()) {
            QModelIndex newIndex = index(castItem(oldIndex.internalPointer()));
            changePersistentIndex(oldIndex, newIndex);
        }
    }
}


QModelIndex DocumentListModel::index(DocumentListItem* item) {
    if (item == nullptr) {
        return {};
    }

    const int row = getRow(item);
    return row < 0 ? QModelIndex() : createIndex(row, 0, item);
}


QModelIndex DocumentListModel::index(Document* document) {
    if (document == nullptr) {
        return {};
    }

    const QString path = document->getPath();
    if (path.isEmpty()) {
        return {};
    }

    for (int i = 0; i < mDocuments.size(); i++) {
        if (mDocuments.at(i)->getPath() == path) {
            return index(i, 0, {});
        }
    }

    return {};
}


int DocumentListModel::getRow(DocumentListItem* item) {
    const QString& itemPath = item->getPath();

    for (int i = 0; i < mDocuments.size(); i++) {
        if (itemPath == mDocuments.at(i)->getPath()) {
            return i;
        }
    }

    return -1;
}


QString DocumentListModel::copyPath(const QModelIndex& index) {
    if (!index.isValid()) {
        return {};
    }

    auto path = getDocumentAt(index)->getPath();
    QApplication::clipboard()->setText(path);

    return path;
}


}   // namespace Ui::Models
