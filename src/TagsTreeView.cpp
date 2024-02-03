#include "TagsTreeView.h"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDrag>
#include <QDragMoveEvent>
#include <QMenu>
#include <QMimeData>
#include <array>
#include <memory>
#include "TagTreeDelegate.h"
#include "TagTreeModel.h"

using Ui::Models::TagTreeModel;
using Ui::Delegates::TagTreeDelegate;

constexpr std::array<const char*, 8> colors{ "default", "green",   "yellow", "orange",
                                             "red",     "magenta", "blue",   "cyan" };

TagsTreeView::TagsTreeView(QWidget* parent) : QTreeView(parent) {
    setHeaderHidden(true);
    setDragDropMode(DragOnly);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setExpandsOnDoubleClick(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    setItemDelegate(new TagTreeDelegate(this));
    connect(this, &TagsTreeView::clicked, this, &TagsTreeView::onClicked);
    connect(this, &TagsTreeView::customContextMenuRequested, this,
            &TagsTreeView::onCustomContextMenuRequested);
}


void TagsTreeView::changeTagColor(const QModelIndex& index, const QString& color) {
    model()->setData(index, color, Qt::ForegroundRole);
}


void TagsTreeView::pinTag(const QModelIndex& index, bool pin) {
    model()->setData(index, pin, TagTreeModel::PinnedTagRole);
}


void TagsTreeView::onClicked([[maybe_unused]] const QModelIndex& index) {
    QStringList tags;
    for (auto ind : selectedIndexes()) {
        if (!ind.isValid()) {
            continue;
        }

        auto tag = model()->data(ind, model()->CompleteNameRole).toString();
        if (!tag.isEmpty()) {
            tags.push_back(tag);
        }
    }

    if (!tags.isEmpty()) {
        emit tagsSelected(tags);
    }
}


TagTreeModel* TagsTreeView::model() {
    return static_cast<TagTreeModel*>(QTreeView::model());
}


void TagsTreeView::onCustomContextMenuRequested(QPoint pos) {
    const auto index = indexAt(pos);
    if (index.internalPointer() == nullptr) {
        return;
    }

    auto isPinned = index.data(TagTreeModel::PinnedTagRole).toBool();
    auto expanded = isExpanded(index);

    auto menu      = std::make_unique<QMenu>();
    auto colorMenu = menu->addMenu(tr("Change the color"));
    auto pin       = menu->addAction((isPinned) ? tr("Unpin") : tr("Pin"));

    for (auto color : colors) {
        const QString iconPath = QStringLiteral(":images/color_") + color;
        auto act               = colorMenu->addAction(QIcon(iconPath), tr(color));
        connect(act, &QAction::triggered, this, [=] { changeTagColor(index, color); });
    }

    auto expandCollapse = menu->addAction(((expanded) ? tr("Collapse") : tr("Expand")));
    menu->addMenu(colorMenu);
    auto copyTag = menu->addAction(tr("Copy tag"));

    connect(pin, &QAction::triggered, this, [=] { pinTag(index, !isPinned); });
    connect(expandCollapse, &QAction::triggered, this, [=] { setExpanded(index, !expanded); });
    connect(copyTag, &QAction::triggered, this, [=] {
        const QString tag = model()->data(index, model()->CompleteNameRole).toString();
        qApp->clipboard()->setText(tag);
    });

    menu->exec(mapToGlobal(pos));
}


void TagsTreeView::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void TagsTreeView::startDrag([[maybe_unused]] Qt::DropActions supportedActions) {
    const QString tag = model()->data(currentIndex(), model()->CompleteNameRole).toString();

    auto mime = new QMimeData;
    mime->setText(tag);
    auto drag = new QDrag(this);
    drag->setMimeData(mime);
    drag->exec(Qt::CopyAction);
}
