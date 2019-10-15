#ifndef ELEMENTH
#define ELEMENTH

#include "baseelement.h"

namespace be = BaseElement;

class Element 
{
public:
    Element(const fs::path& path);
    Element(const fs::path& path, const std::string& title, 
            const Tags& tags, const bool& pinned=false, 
            const bool& favorited=false, const bool& deleted=false);
    Element(const Element& other);
    inline bool operator==(const Element& other) { return (m_path == other.m_path);  }
    inline bool operator!=(const Element& other) { return !(m_path == other.m_path); }

    [[nodiscard]] inline fs::path path()     const  {  return m_path;     }
    [[nodiscard]] inline std::string title() const  {  return m_title;    }
    [[nodiscard]] inline Tags tags()         const  {  return m_tags;     }
    [[nodiscard]] inline bool pinned()       const  {  return m_pinned;   }
    [[nodiscard]] inline bool favorited()    const  {  return m_favorited;}
    [[nodiscard]] inline bool deleted()      const  {  return m_deleted;  }
    [[nodiscard]] inline StringList getHeader() const{ return m_header;   }

    inline void setPath(const fs::path& path)       {  m_path = path;      }
    inline void setTitle(const std::string& title)  {  m_title = title;    }
    inline void setTags(const Tags &tags)           {  m_tags = tags;      }
    inline void setPinned(const bool &pinned)       {  m_pinned = pinned;  }
    inline void setFavorited(const bool& favorited) {  m_favorited = favorited;}
    inline void setDeleted(const bool& deleted)     {  m_deleted = deleted;}
    inline void setLocalHeader(const StringList& head){m_header = head;    }

    /**
     * construct the object (this) by calling all the appropriate functions
     */
    void setup(const fs::path& path);
    inline void reload()               { setup( path() );   }
    inline void reloadHeader()         { m_header = be::getHeader(m_path); }
    static ElementsList constructElementList(const PathsList& f);
    /**
     * Check if a line exists in the file's header
     */
    [[nodiscard]] inline bool hasTagsLine()     const { return be::hasTagsKey(m_header);  }
    [[nodiscard]] inline bool hasTitleLine()    const { return be::hasTitleKey(m_header); }
    [[nodiscard]] inline bool hasPinnedLine()   const { return be::hasPinnedKey(m_header);}
    [[nodiscard]] inline bool hasFavoritedLine()const { return be::hasFavoritedKey(m_header);}
    [[nodiscard]] inline bool hasDeletedLine()  const { return be::hasDeletedKey(m_header);}
    /**
     * append a non existing line to the file's header
     */
    void addPinnedLine(const bool& val);
    void addFavoritedLine(const bool& val);
    void addDeletedLine(const bool& val);
    void addTagsLine(const StringList& list);
    /**
     * Change the value of key inside the file
     * and reload it to the local variable
     */
    void changeTitle(std::string title);
    void changePinned(bool pinned);
    void changeFavorited(bool favorited);
    void changeDeleted(bool deleted);
    void overrideTags(const StringList& list);
    inline void changeTags(const StringList& list)  {  overrideTags(list);   }
    /**
     * abstraction function to append a tag into a file
     * update the local variable after appending
     */
    bool appendTag(std::string tag);
    inline void appendTags(const StringList& tags)  { for (auto& i : tags) appendTag(i); }
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
    inline void initTags()               {   initTags(be::getHeader(m_path));   }
    void initTags(const StringList& header);

private:
    fs::path m_path;
    std::string m_title;
    Tags m_tags;
    bool m_pinned;
    bool m_favorited;
    bool m_deleted;
    StringList m_header;
};


#endif // Element_H
