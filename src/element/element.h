#ifndef ELEMENTH
#define ELEMENTH

#include "baseelement.h"

namespace be = BaseElement;

class Element 
{
/****************************************************/



public:
    Element(const fs::path& path);
    Element(const fs::path& path, const std::string& title, const Tags& tags, const bool& pinned=false, const bool& favorited=false, const bool& deleted=false);
    Element(const Element& other);
    inline bool operator==(const Element& other)         {   return equalTo(other);        }



    // from BaseClass
    // accessers
    [[nodiscard]] inline fs::path path()     const      {    return m_path;     }
    [[nodiscard]] inline std::string title() const      {    return m_title;    }
    [[nodiscard]] inline Tags tags()         const      {    return m_tags;     }
    [[nodiscard]] inline bool pinned()       const      {    return m_pinned;   }
    [[nodiscard]] inline bool favorited()    const      {    return m_favorited;}
    [[nodiscard]] inline bool deleted()      const      {    return m_deleted;  }
    // modifiers
    inline void setPath(const fs::path& path)           {    m_path = path;      }
    inline void setTitle(const std::string& title)      {    m_title = title;    }
    inline void setTags(const Tags &tags)               {    m_tags = tags;      }
    inline void setPinned(const bool &pinned)           {    m_pinned = pinned;  }
    inline void setFavorited(const bool& favorited)     {    m_favorited = favorited;}
    inline void setDeleted(const bool& deleted)         {    m_deleted = deleted;}



    inline void reload()                                 {   setup( path() );              }
    // inline bool equalTo(Element* e) const                {   return ( m_path == e->m_path);  }
    inline bool equalTo(const Element& e)const           {   return ( m_path == e.m_path); }

    static ElementsList construct_list_elements(const PathsList& f);
    /**
     * Get a specific raw line from header
     */
    [[nodiscard]] inline StringList getHeaderFromFile()       { return be::getHeader(m_path); }
    [[nodiscard]] inline std::string getTitleLineFromHeader() { return be::find_title_inheader(getHeaderFromFile());   }
    [[nodiscard]] inline std::string getTagsLineFromHeader()  { return be::find_tags_inheader(getHeaderFromFile());    }
    [[nodiscard]] inline std::string getPinnedLineFromHeader(){ return be::find_pinned_inheader(getHeaderFromFile());  }
    [[nodiscard]] inline std::string getFavoritedLineFromHeader() { return be::find_favorite_inheader(getHeaderFromFile());}
    [[nodiscard]] inline std::string getDeletedLineFromHeader()   { return be::find_deleted_inheader(getHeaderFromFile()); }
    /**
     * get the value of a specific header item
     */
    [[nodiscard]] inline std::string getTitleValueFromHeader(){ return be::extract_title( getTitleLineFromHeader() );      }
    [[nodiscard]] inline bool getPinnedValueFromHeader()      { return be::extract_pinned( getPinnedLineFromHeader() );    }
    [[nodiscard]] inline bool getFavoritedValueFromHeader()   { return be::extract_favorited( getFavoritedLineFromHeader() );}
    [[nodiscard]] inline bool getDeletedValueFromHeader()     { return be::extract_deleted( getDeletedLineFromHeader() );  }
    [[nodiscard]] inline StringList getUnparsedTagsValuesFromHeader() { return be::extract_tags( getTagsLineFromHeader() );        }
    [[nodiscard]] inline Tags getParsedTagsValueFromHeader()  { return be::parse_tags( getUnparsedTagsValuesFromHeader() );}
    /**
     * Check if a line exists in the file's header
     */
    [[nodiscard]] inline bool hasTagsLine()      const { return be::hasTagItem(path());       }
    [[nodiscard]] inline bool hasTitleLine()     const { return be::hasTitleItem(m_path);     }
    [[nodiscard]] inline bool hasPinnedLine()    const { return be::hasPinnedItem(m_path);    }
    [[nodiscard]] inline bool hasFavoritedLine() const { return be::hasFavoritedItem(m_path); }
    [[nodiscard]] inline bool hasDeletedLine()   const { return be::hasDeletedItem(m_path);   }
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
    inline bool findReplace(const std::string& old_str, const std::string& new_str)
        { return BaseElement::replace(old_str, new_str, m_path); }
    /***
     * Reload individual items (read the file to set the local variables)
     */
    inline void reloadTitle()            {   setTitle(getTitleValueFromHeader());         }
    inline void reloadPinned()           {   setPinned(getPinnedValueFromHeader());       }
    inline void reloadFavorited()        {   setFavorited(getFavoritedValueFromHeader()); }
    inline void reloadDeleted()          {   setDeleted(getDeletedValueFromHeader());     }
    inline void initTags()               {   initTags(be::getHeader(m_path));                 }
    void initTags(const StringList& header);
    /**
     * construct the object (this) by calling all the appropriate functions
     */
    void setup(const fs::path& path);

private:
    fs::path m_path;
    std::string m_title;
    Tags m_tags;
    bool m_pinned;
    bool m_favorited;
    bool m_deleted;


};


#endif // Element_H
