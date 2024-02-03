#ifndef DOCUMENT__H
#define DOCUMENT__H

#include <QVector>
#include "DocumentUtils.h"

namespace Doc {
/**
 * representation of single document
 */
class Document final {
public:
    Document() = default;
    explicit Document(QString path);
    Document(const Document&)            = delete;
    Document& operator=(const Document&) = delete;
    Document(Document&&)                 = default;
    Document& operator=(Document&&)      = default;
    void load();
    static QVector<Document*> constructDocumentList(const QStringVector& paths);
    [[nodiscard]] QString getPath() const;
    [[nodiscard]] QString getTitle() const;
    [[nodiscard]] QStringVector getTags() const;
    [[nodiscard]] QStringVector getHeader() const;
    [[nodiscard]] bool isPinned() const;
    [[nodiscard]] bool isFavorited() const;
    [[nodiscard]] bool isDeleted() const;
    [[nodiscard]] bool containsExactTag(const QString& tag) const;
    [[nodiscard]] bool containsTag(const QStringVector& tag) const;
    [[nodiscard]] bool containsAllTags(const QStringList& tags) const;
    void setPath(const QString& path);
    void setTitle(const QString& title);
    void setPinned(bool pinned);
    void setFavorited(bool favorited);
    void setDeleted(bool deleted);
    void setHeader(const QStringVector& header);
    void delTag(const QString& tag);
    void addTag(const QString& tag);

    friend bool operator==(const Document& lhs, const Document& rhs);
    friend bool operator<(const Document& lhs, const Document& rhs);

private:
    QString mPath;
    QString mTitle;
    bool mPinned{ false };
    bool mFavorited{ false };
    bool mDeleted{ false };
    QStringVector mTags;
    QStringVector mHeader;
};

}   // namespace Doc
#endif   // DOCUMENT__H
