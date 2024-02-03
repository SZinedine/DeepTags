#ifndef DOCUMENTLISTDELEGATE__H
#define DOCUMENTLISTDELEGATE__H


#include <QStyledItemDelegate>

namespace Ui::Delegates {

class DocumentListDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit DocumentListDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const final;
};
}   // namespace Ui::Delegates

#endif   // DOCUMENTLISTDELEGATE__H
