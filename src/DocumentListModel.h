#ifndef DOCUMENTLISTMODEL__H
#define DOCUMENTLISTMODEL__H

#include <QAbstractItemModel>
#include <QVector>

namespace Doc {
class Document;
}

namespace Ui::Models {

class DocumentListItem;

class DocumentListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum CustomRoles {
        TitleRole = Qt::UserRole,
        PathRole,
        PinnedRole,
        FavoritedRole,
        DeletedRole,
        PermanentlyDeletedRole,
        TagsRole,
        TagAddRole,
        TagDeleteRole
    };
    explicit DocumentListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent) const final;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const final;
    bool setData(const QModelIndex& index, const QVariant& value, int role) final;
    bool setData(Doc::Document* document, const QVariant& value, int role);
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const final;
    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) final;
    Doc::Document* getDocumentAt(int row);
    Doc::Document* getDocumentAt(const QModelIndex& index);
    void reset();
    void setDocuments(const QVector<Doc::Document*>& documents, const QStringList& activeTag);
    void addDocument(Doc::Document* document);
    void addDocuments(const QVector<Doc::Document*>& documents);
    void removeRow(int row);
    void updatePersistentIndexes();
    using QAbstractListModel::index;
    QModelIndex index(DocumentListItem* item);
    QModelIndex index(Doc::Document* document);
    int getRow(DocumentListItem* item);
    QString copyPath(const QModelIndex& index);
    [[nodiscard]] inline auto getActiveTag() const { return mActiveTag; }

public slots:
    void onDocumentChanged(const QModelIndex& index, int role);

signals:
    void documentPermanentlyDeleted(Doc::Document* document);
    void documentDeleted(Doc::Document* document);
    void tagDeleted(QString tag);
    void tagAdded(QString tag);
    void numberOfDocumentsChanged(int number);

private:
    QVector<Doc::Document*> mDocuments;
    /**
     * empty `mActiveTag` means either no selected tag or special tag selected
     */
    QStringList mActiveTag;
};

}   // namespace Ui::Models

#endif   // DOCUMENTLISTMODEL__H
