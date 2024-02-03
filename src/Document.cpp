#include "Document.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentMap>
#include "Benchmark.h"
#include "DocumentUtils.h"

namespace Doc {

Document::Document(QString path) : mPath{ std::move(path) } {
    load();
}


void Document::load() {
    mHeader = Utils::getHeader(mPath);
    if (mHeader.empty()) {
        mTitle     = QFileInfo(mPath).baseName();
        mPinned    = false;
        mFavorited = false;
        mDeleted   = false;
        return;
    }

    bool hasTitle = std::any_of(mHeader.cbegin(), mHeader.cend(), [](const QString& line) {
        return line.startsWith(QStringLiteral("title:"));
    });
    mTitle     = hasTitle ? Utils::getHeaderValue(titleKey, mHeader) : QFileInfo(mPath).baseName();
    mPinned    = Utils::getHeaderValue((pinnedKey), mHeader) == trueStr;
    mFavorited = Utils::getHeaderValue(favoritedKey, mHeader) == trueStr;
    mDeleted   = Utils::getHeaderValue(deletedKey, mHeader) == trueStr;
    mTags      = Utils::splitTags(mHeader);
}


QVector<Document*> Document::constructDocumentList(const QStringVector& paths) {
    Benchmark b("Document construction");

    QVector<Document*> docs;
    QMutex mutex;
    auto pushNewDocument = [&docs, &mutex](const QString& p) {
        auto doc = new Document(p);
        QMutexLocker lock(&mutex);
        docs.push_back(doc);
    };

    QtConcurrent::blockingMap(paths, pushNewDocument);

    return docs;
}


QString Document::getPath() const {
    return mPath;
}


QString Document::getTitle() const {
    return mTitle;
}


QStringVector Document::getTags() const {
    return mTags;
}


QStringVector Document::getHeader() const {
    return mHeader;
}


bool Document::isPinned() const {
    return mPinned;
}


bool Document::isFavorited() const {
    return mFavorited;
}


bool Document::isDeleted() const {
    return mDeleted;
}


bool Document::containsExactTag(const QString& tag) const {
    return mTags.contains(tag);
}


bool Document::containsTag(const QStringVector& tag) const {
    for (const QString& t : mTags) {
        const QStringVector tagLineChain = Utils::deconstructTag(t);

        if (tagLineChain.size() < tag.size()) {
            continue;
        }

        if (std::equal(tag.cbegin(), tag.cend(), tagLineChain.cbegin(),
                       tagLineChain.cbegin() + tag.size())) {
            return true;
        }
    }

    return false;
}


bool Document::containsAllTags(const QStringList& tags) const {
    return std::any_of(tags.begin(), tags.end(),
                       [this](const QString& t) { return containsTag(Utils::deconstructTag(t)); });
}


void Document::setPath(const QString& path) {
    if (!path.isEmpty()) {
        mPath = path;
    }
}


void Document::setTitle(const QString& title) {
    if (!title.isEmpty()) {
        mTitle = title;
    }
}


void Document::setPinned(bool pinned) {
    mPinned = pinned;
}


void Document::setFavorited(bool favorited) {
    mFavorited = favorited;
}


void Document::setDeleted(bool deleted) {
    mDeleted = deleted;
}


void Document::setHeader(const QStringVector& header) {
    mHeader = header;
}


void Document::addTag(const QString& tag) {
    if (!tag.isEmpty()) {
        mTags.append(tag.simplified());
    }
}


void Document::delTag(const QString& tag) {
    mTags.removeAll(tag);
}


bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.mPath == rhs.mPath;
}


bool operator<(const Document& lhs, const Document& rhs) {
    if (lhs.mPinned && !rhs.mPinned) {
        return true;
    } else if (rhs.mPinned && !lhs.mPinned) {
        return false;
    }

    return lhs.mTitle < rhs.mTitle;
}


}   // namespace Doc
