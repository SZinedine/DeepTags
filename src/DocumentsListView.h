#ifndef DOCUMENTLISTVIEW__H
#define DOCUMENTLISTVIEW__H

#include <QListView>

class QShortcut;

namespace Ui::Models {
class DocumentListModel;
}

namespace Ui::Delegates {
class DocumentListDelegate;
}

namespace Doc {
class Document;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class DocumentsListView;
}
QT_END_NAMESPACE


class DocumentsListView : public QListView {
    Q_OBJECT
public:
    explicit DocumentsListView(QWidget* parent = nullptr);
    ~DocumentsListView() override;

public slots:
    void openCurrentDocument();

private slots:
    void onCustomContextMenuRequested(const QPoint& pos);
    void onDocumentSelected(const QModelIndex& index);
    void addTagDialog(const QModelIndex& index);
    void openDocumentAt(const QModelIndex& index);
    void launchDocumentInfoDialog(const QModelIndex& index);
    void permanentlyDelete(const QModelIndex& index);

private:
    void dropEvent(QDropEvent* event) final;
    void dragMoveEvent(QDragMoveEvent* event) final;
    void dragEnterEvent(QDragEnterEvent* event) final;
    Ui::Models::DocumentListModel* model();
    void setCurrentDocument(Doc::Document* document);
    void toggleBool(const QModelIndex& index, int role);

signals:
    void documentSelected(Doc::Document* document);
    void documentToOpen(const QString& path, const QString& editor = "");

private:
    Ui::Delegates::DocumentListDelegate* mDeletegate;
    std::vector<std::unique_ptr<QShortcut>> mShortcuts;
};


#endif   // DOCUMENTLISTVIEW__H
