#ifndef TAGTREEITEM__H
#define TAGTREEITEM__H

#include <QHash>
#include <QString>
#include <QVector>

class QVariant;

namespace Ui::Models {

/**
 * this represents a single particle in the tag tree
 */
class TagTreeItem final {
public:
    explicit TagTreeItem(QString name, TagTreeItem* parent = nullptr);
    TagTreeItem(const TagTreeItem&)            = delete;
    TagTreeItem& operator=(const TagTreeItem&) = delete;
    TagTreeItem(TagTreeItem&&)                 = default;
    TagTreeItem& operator=(TagTreeItem&&)      = default;
    ~TagTreeItem();
    [[nodiscard]] QString getName() const;
    [[nodiscard]] QString getCompleteName() const;
    [[nodiscard]] QVector<TagTreeItem*> getChildren();
    [[nodiscard]] TagTreeItem* getParent();
    void addChild(TagTreeItem* child);
    TagTreeItem* getChild(int index);
    TagTreeItem* getChild(const QString& name);
    void removeChild(const QString& name);
    [[nodiscard]] int row() const;
    void setName(const QString& name);
    void setColor(const QString& color) const;
    void setPinned(bool pinned);
    void sortChildren();
    [[nodiscard]] bool isSpecialTag() const;
    [[nodiscard]] bool isPinned() const;
    [[nodiscard]] QString getColor() const;

    friend bool operator<(const TagTreeItem& lhs, const TagTreeItem& rhs);
    friend bool operator>(const TagTreeItem& lhs, const TagTreeItem& rhs);
    friend bool operator<=(const TagTreeItem& lhs, const TagTreeItem& rhs);
    friend bool operator>=(const TagTreeItem& lhs, const TagTreeItem& rhs);

    static void refreshFromSettings();

public:
    inline static const QVector<QString> specialTags{ "All Notes", "Favorite", "Trash",
                                                      "Untagged" };
    ;
    inline static QVector<QString> pinnedTags{};
    inline static QHash<QString, QVariant> coloredTags{};

private:
    QString mName;
    TagTreeItem* mParent;
    QVector<TagTreeItem*> mChildren;
};

}   // namespace Ui::Models

#endif   // TAGTREEITEM__H
