#ifndef ELEMENTH
#define ELEMENTH

#include "baseelement.h"

namespace be = BaseElement;

class Element {
public:
    explicit Element(const QString& path);
    Element(const Element& other);
    Element(Element&& other) = default;
    inline bool operator==(const Element& other) { return (m_path == other.m_path); }
    inline bool operator!=(const Element& other) { return !(m_path == other.m_path); }

    [[nodiscard]] inline QString path() const { return m_path; }
    [[nodiscard]] inline QString title() const { return m_title; }
    [[nodiscard]] inline Tags tags() const { return m_tags; }
    [[nodiscard]] inline bool pinned() const { return m_pinned; }
    [[nodiscard]] inline bool favorited() const { return m_favorited; }
    [[nodiscard]] inline bool deleted() const { return m_deleted; }
    [[nodiscard]] inline StringList getHeader() const { return m_header; }
    [[nodiscard]] inline bool untagged() const { return tags().empty(); }

    inline void setPath(const QString& path) { m_path = path; }
    inline void setTitle(const QString& title) { m_title = title; }
    inline void setTags(const Tags& tags) { m_tags = tags; }
    inline void setPinned(const bool& pinned) { m_pinned = pinned; }
    inline void setFavorited(const bool& favorited) { m_favorited = favorited; }
    inline void setDeleted(const bool& deleted) { m_deleted = deleted; }
    inline void setLocalHeader(const StringList& head) { m_header = head; }

    /**
     * construct the object (this) by calling all the appropriate functions
     */
    void setup();
    inline void reload() { setup(); }
    inline void reloadHeader() { m_header = be::getHeader(m_path); }
    static ElementsList constructElementList(const PathsList& f);
    /**
     * Check if a line exists in the file's header
     */
    [[nodiscard]] inline bool hasTagsLine() const { return be::hasTagsKey(m_header); }
    [[nodiscard]] inline bool hasTitleLine() const { return be::hasTitleKey(m_header); }
    [[nodiscard]] inline bool hasPinnedLine() const { return be::hasPinnedKey(m_header); }
    [[nodiscard]] inline bool hasFavoritedLine() const { return be::hasFavoritedKey(m_header); }
    [[nodiscard]] inline bool hasDeletedLine() const { return be::hasDeletedKey(m_header); }
    /**
     * append a non existing line to the file's header
     */
    void addTitleLine(const QString& title);
    void addPinnedLine(const bool& val);
    void addFavoritedLine(const bool& val);
    void addDeletedLine(const bool& val);
    void addTagsLine(const StringList& list);
    /**
     * Change the value of key inside the file
     * and reload it to the local variable
     */
    void changeTitle(const QString& title);
    void changePinned(bool pinned);
    void changeFavorited(bool favorited);
    void changeDeleted(bool deleted);
    void overrideTags(const StringList& list);
    inline void changeTags(const StringList& list) { overrideTags(list); }
    /**
     * abstraction function to append a tag into a file
     * update the local variable after appending
     */
    bool appendTag(QString tag);
    inline void appendTags(const StringList& tags) {
        for (auto& i : tags) appendTag(i);
    }
    /**
     * remove entirely a specific item (line) from the file's header
     */
    void removePinnedLine();
    void removeFavoritedLine();
    void removeDeletedLine();
    void removeTagsLine();

private:
    /***
     * Reload individual items (read the file to set the local variables)
     */
    inline void loadTags() { loadTags(be::getHeader(m_path)); }
    void loadTags(const StringList& header);

private:
    QString m_path;
    QString m_title;
    Tags m_tags;
    bool m_pinned;
    bool m_favorited;
    bool m_deleted;
    StringList m_header;
};


#endif   // Element_H
