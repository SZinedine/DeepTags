#include "DocumentListDelegate.h"
#include <QDebug>
#include "DocumentListModel.h"

namespace Ui::Delegates {

DocumentListDelegate::DocumentListDelegate(QObject* parent) : QStyledItemDelegate(parent) {}


void DocumentListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const {
    QStyleOptionViewItem myOption(option);

    myOption.features = QStyleOptionViewItem::HasDisplay | QStyleOptionViewItem::HasDecoration;

    myOption.decorationPosition = QStyleOptionViewItem::Right;

    const bool isPinned    = index.data(Models::DocumentListModel::PinnedRole).toBool();
    const bool isFavorited = index.data(Models::DocumentListModel::FavoritedRole).toBool();

    static const QIcon mPinIcon(QStringLiteral(":images/pin.png"));
    static const QIcon mFavIcon(QStringLiteral(":images/favorite.png"));
    static const QIcon mFavPinIcon(QStringLiteral(":images/favpin.png"));

    if (isPinned && isFavorited) {
        myOption.icon = mFavPinIcon;
    } else if (isPinned) {
        myOption.icon = mPinIcon;
    } else if (isFavorited) {
        myOption.icon = mFavIcon;
    }

    QStyledItemDelegate::paint(painter, myOption, index);
}


}   // namespace Ui::Delegates
