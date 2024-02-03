#ifndef TAGTREEMODEL__H
#define TAGTREEMODEL__H

#include <QAbstractItemModel>

namespace Doc {
class Document;
}

namespace Ui::Models {

class TagTreeItem;

class TagTreeModel final : public QAbstractItemModel {
    Q_OBJECT
public:
    enum CustomRoles { NameRole = Qt::UserRole, CompleteNameRole, SpecialTagRole, PinnedTagRole };

    explicit TagTreeModel(QVector<Doc::Document*>* documents = nullptr, QObject* parent = nullptr);
    ~TagTreeModel() override;

public slots:
    void deleteTag(const QString& tag);
    void addTag(const QString& tag);
    void reset();
    void setDocuments(QVector<Doc::Document*>* documents);

public:
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const final;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const final;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final;
    [[nodiscard]] QModelIndex index(int row, int column,
                                    const QModelIndex& parent = QModelIndex()) const final;
    QModelIndex index(TagTreeItem* item) const;
    [[nodiscard]] QModelIndex parent(const QModelIndex& index) const final;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const final;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const final;
    bool setData(const QModelIndex& index, const QVariant& value, int role) final;
    [[nodiscard]] Qt::DropActions supportedDropActions() const final;

private:
    void setupTags();
    void setupPermanentTags();
    void updatePersistentIndexes();
    bool tagExists(const QString& tag);
    void sortTags();
    [[nodiscard]] static TagTreeItem* itemAt(const QModelIndex& index);

private:
    TagTreeItem* mRootItem;
    QVector<Doc::Document*>* mDocuments;
};

}   // namespace Ui::Models

#endif   // TAGTREEMODEL__H
