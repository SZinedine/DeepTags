#include "TagTreeModel.h"
#include <QApplication>
#include <QColor>
#include <QIcon>
#include "Benchmark.h"
#include "Document.h"
#include "DocumentUtils.h"
#include "TagTreeItem.h"

#define castItem(x) static_cast<TagTreeItem*>(x)

namespace Ui::Models {

TagTreeModel::TagTreeModel(QVector<Doc::Document*>* documents, QObject* parent)
    : QAbstractItemModel{ parent }, mRootItem{ new TagTreeItem("") }, mDocuments{ documents } {
    TagTreeItem::refreshFromSettings();
    setDocuments(documents);
}


TagTreeModel::~TagTreeModel() {
    delete mRootItem;
}


void TagTreeModel::setDocuments(QVector<Doc::Document*>* documents) {
    mDocuments = documents;
    setupTags();
}


void TagTreeModel::setupPermanentTags() {
    if (mRootItem == nullptr) {
        mRootItem = new TagTreeItem("");
    }

    for (const auto& tag : TagTreeItem::specialTags) {
        if (mRootItem->getChild(tag) == nullptr) {
            new TagTreeItem(tag, mRootItem);
        }
    }
}


void TagTreeModel::reset() {
    beginResetModel();

    mDocuments = nullptr;
    delete mRootItem;
    mRootItem = nullptr;

    endResetModel();
}


void TagTreeModel::setupTags() {
    Benchmark b("tags loading");
    setupPermanentTags();

    if (mDocuments == nullptr || mDocuments->empty()) {
        return;
    }

    for (const auto& doc : *mDocuments) {
        if (doc->isDeleted()) {
            continue;
        }

        for (auto& tagChain : doc->getTags()) {
            TagTreeItem* previous = mRootItem;

            for (auto& tag : Doc::Utils::deconstructTag(tagChain)) {
                if (auto prev = previous->getChild(tag); prev != nullptr) {
                    previous = prev;
                    continue;
                }

                auto newItem = new TagTreeItem(tag, previous);
                previous     = newItem;
            }
        }
    }

    sortTags();
}


void TagTreeModel::addTag(const QString& tagChain) {
    if (tagChain.isEmpty()) {
        return;
    }

    emit layoutAboutToBeChanged();

    TagTreeItem* previous = mRootItem;

    for (auto& tag : Doc::Utils::deconstructTag(tagChain)) {
        if (auto prev = previous->getChild(tag); prev != nullptr) {
            previous = prev;
            continue;
        }

        auto newItem = new TagTreeItem(tag, previous);
        previous->sortChildren();
        int i = newItem->row();
        beginInsertRows(index(previous), i, i);
        previous = newItem;
        endInsertRows();
    }

    updatePersistentIndexes();
    emit layoutChanged();
}


TagTreeItem* getTagTreeItem(const QStringVector& tagSeq, TagTreeItem* root) {
    if (root == nullptr) {
        return nullptr;
    }

    TagTreeItem* res = root;
    for (const auto& tag : tagSeq) {
        if (auto child = res->getChild(tag)) {
            res = child;
        } else {
            return root;
        }
    }

    return res;
}


void TagTreeModel::deleteTag(const QString& tagChain) {
    if (tagChain.isEmpty() || tagExists(tagChain)) {
        return;
    }

    const auto tagSeq = Doc::Utils::deconstructTag(tagChain);
    TagTreeItem* item = getTagTreeItem(tagSeq, mRootItem);

    TagTreeItem* parent = item->getParent();
    auto parentIndex    = index(parent);

    if (parent == nullptr) {
        return;
    }

    int row = item->row();
    emit layoutAboutToBeChanged();
    beginRemoveRows(parentIndex, row, row);
    item->getParent()->removeChild(item->getName());
    endRemoveRows();

    emit layoutChanged();

    // delete parent of item if it is empty
    const QString tag = parent->getCompleteName();
    if (1 < tagSeq.size() && !tagExists(tag)) {
        deleteTag(tag);
    }
}


bool TagTreeModel::tagExists(const QString& tag) {
    const QStringVector tagChain = Doc::Utils::deconstructTag(tag);
    return std::any_of(mDocuments->cbegin(), mDocuments->cend(),
                       [&tagChain](auto t) { return !t->isDeleted() && t->containsTag(tagChain); });
    // for (const Doc::Document* const t : *mDocuments) {
    //     if (!t->isDeleted() && t->containsTag(tagChain)) {
    //         return true;
    //     }
    // }

    // return false;
}


void TagTreeModel::sortTags() {
    if (mRootItem != nullptr) {
        mRootItem->sortChildren();
    }
}


int TagTreeModel::columnCount([[maybe_unused]] const QModelIndex& parent) const {
    return 1;
}


int TagTreeModel::rowCount(const QModelIndex& parent) const {
    if (0 < parent.column()) {
        return 0;
    }

    auto parentItem = (parent.isValid()) ? itemAt(parent) : mRootItem;
    if (parentItem != nullptr) {
        return parentItem->getChildren().size();
    }

    return 0;
}


QVariant TagTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return itemAt(index)->getName();
    case CompleteNameRole:
        return itemAt(index)->getCompleteName();
    case SpecialTagRole:
        return TagTreeItem::specialTags.contains(itemAt(index)->getCompleteName());
    case Qt::ForegroundRole:
        if (const QString color = itemAt(index)->getColor(); !color.isEmpty()) {
            return QColor(color);
        }

        return {};
    case PinnedTagRole:
        return itemAt(index)->isPinned();
    case Qt::DecorationRole: {
        if (const QString n = data(index, NameRole).toString(); n == "All Notes") {
            return QIcon(QStringLiteral(":images/all_notes.png"));
        } else if (n == "Trash") {
            return QIcon(QStringLiteral(":images/trash.png"));
        } else if (n == "Favorite") {
            return QIcon(QStringLiteral(":images/star.png"));
        } else if (n == "Untagged") {
            return QIcon(QStringLiteral(":images/untagged.png"));
        } else if (n == "Notebook") {
            return QIcon(QStringLiteral(":images/notebook.png"));
        }
    }
    }

    return {};
}


bool TagTreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    auto item    = itemAt(index);
    bool changed = false;

    if (item == nullptr) {
        return false;
    }

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        item->setName(value.toString());
        changed = true;
        break;
    case Qt::ForegroundRole:
        item->setColor(value.toString());
        changed = true;
        break;
    case PinnedTagRole:
        item->setPinned(value.toBool());
        emit layoutAboutToBeChanged();
        sortTags();
        updatePersistentIndexes();
        emit layoutChanged();
        changed = true;
        break;
    }

    if (changed) {
        emit dataChanged(index, index, { role });
    }

    return changed;
}


void TagTreeModel::updatePersistentIndexes() {
    QModelIndexList persistentList = persistentIndexList();

    for (auto oldIndex : persistentList) {
        if (oldIndex.isValid()) {
            QModelIndex newIndex = index(castItem(oldIndex.internalPointer()));
            changePersistentIndex(oldIndex, newIndex);
        }
    }
}


QModelIndex TagTreeModel::index(TagTreeItem* item) const {
    if (item == nullptr) {
        return {};
    }

    const int row = item->row();
    return row < 0 ? QModelIndex() : createIndex(row, 0, item);
}


QModelIndex TagTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    TagTreeItem* parentItem = parent.isValid() ? castItem(parent.internalPointer()) : mRootItem;

    TagTreeItem* childItem = parentItem->getChild(row);
    if (childItem != nullptr) {
        return createIndex(row, column, childItem);
    }

    return {};
}


QModelIndex TagTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) {
        return {};
    }

    TagTreeItem* childItem  = castItem(index.internalPointer());
    TagTreeItem* parentItem = childItem->getParent();

    if (parentItem == mRootItem) {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem);
}


QHash<int, QByteArray> TagTreeModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[NameRole]         = "name";
    names[CompleteNameRole] = "completeName";
    names[SpecialTagRole]   = "specialTag";
    names[PinnedTagRole]    = "pinned";

    return names;
}


Qt::ItemFlags TagTreeModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }

    return Qt::ItemIsDropEnabled | defaultFlags;
}


Qt::DropActions TagTreeModel::supportedDropActions() const {
    return Qt::CopyAction;
}


TagTreeItem* TagTreeModel::itemAt(const QModelIndex& index) {
    if (index.isValid()) {
        return castItem(index.internalPointer());
    }

    return nullptr;
}
}   // namespace Ui::Models
