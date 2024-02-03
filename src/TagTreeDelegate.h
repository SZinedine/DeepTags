#ifndef TAGTREEDELEGATE__h
#define TAGTREEDELEGATE__h

#include <QStyledItemDelegate>

namespace Ui::Delegates {

class TagTreeDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TagTreeDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const final;
};
}   // namespace Ui::Delegates


#endif   // TAGTREEDELEGATE__h
