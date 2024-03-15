#ifndef TAGSTREEVIEW__H
#define TAGSTREEVIEW__H

#include <QTreeView>

// namespace Ui {

namespace Ui::Models {
class TagTreeModel;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class TagsTreeView;
}
QT_END_NAMESPACE

class TagsTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit TagsTreeView(QWidget* parent = nullptr);
    ~TagsTreeView() override = default;

public slots:
    void changeTagColor(const QModelIndex& index, const QString& color);
    void pinTag(const QModelIndex& index, bool pin);

public slots:
    void onClicked(const QModelIndex& index);

private:
    Ui::Models::TagTreeModel* model();
    void onCustomContextMenuRequested(QPoint pos);
    void dragMoveEvent(QDragMoveEvent* event) final;
    void startDrag(Qt::DropActions supportedActions) final;

signals:
    void tagsSelected(const QStringList& tags);
};

// }   // namespace Ui
#endif   // TAGSTREEVIEW__H
