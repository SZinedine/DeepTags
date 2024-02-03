#include "TagTreeDelegate.h"
#include "TagTreeModel.h"


namespace Ui::Delegates {

TagTreeDelegate::TagTreeDelegate(QObject* parent) : QStyledItemDelegate(parent) {}


void TagTreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const {
    QStyleOptionViewItem myOption(option);
    myOption.features = QStyleOptionViewItem::HasDisplay;

    if (index.data(Ui::Models::TagTreeModel::PinnedTagRole).toBool()) {   // pinned items are bold
        myOption.font.setBold(true);
    }

    QStyledItemDelegate::paint(painter, myOption, index);
}


}   // namespace Ui::Delegates
