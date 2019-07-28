#ifndef ELEMENTH
#define ELEMENTH

#include "baseelement.h"


class Element : public BaseElement
{
public:
    Element(const fs::path& path);
    Element(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned=false, const bool& favorited=false);
    Element(const Element& other);
    bool operator==(const Element& other) { return equalTo(other); }
    inline void reload() 						{  	setup( path() );  				}
    inline bool equalTo(Element* e) const		{	return ( m_path == e->m_path);	}
    inline bool equalTo(const Element& e)const	{	return ( m_path == e.m_path);	}

    static ElementsList construct_list_elements(const PathsList& f);
    /**
     * Get a specific raw line from header
     */
    inline StringList getHeaderFromFile()			{	return getHeader(m_path);	}
    inline std::string getTitleLineFromHeader()	 	{	return find_title_inheader(getHeaderFromFile());		}
    inline std::string getTagsLineFromHeader()		{	return find_tags_inheader(getHeaderFromFile());			}
    inline std::string getPinnedLineFromHeader()	{	return find_pinned_inheader(getHeaderFromFile());		}
    inline std::string getFavoritedLineFromHeader()	{	return find_favorite_inheader(getHeaderFromFile());		}
    /**
     * get the value of a specific header item
     */
    inline std::string getTitleValueFromHeader()	{	return extract_title( getTitleLineFromHeader() );		}
    inline bool getPinnedValueFromHeader()			{	return extract_pinned( getPinnedLineFromHeader() );		}
    inline bool getFavoritedValueFromHeader()		{	return extract_favorited( getFavoritedLineFromHeader() );}
    inline StringList getUnparsedTagsValuesFromHeader()	{	return extract_tags( getTagsLineFromHeader() );		}
    inline Tags getParsedTagsValueFromHeader()  	{	return parse_tags( getUnparsedTagsValuesFromHeader() );	}
    /**
     * Check if a line exists in the file's header
     */
    inline bool hasTagsLine() const		{	return hasTagItem(path());		}
    inline bool hasTitleLine() const	{	return hasTitleItem(m_path);	}
    inline bool hasPinnedLine() const	{	return hasPinnedItem(m_path);	}
    inline bool hasFavoritedLine()const {	return hasFavoritedItem(m_path);}
    /**
     * append a non existing line to the file's header
     */
    void addPinnedLine(const bool& val);
    void addFavoritedLine(const bool& val);
    void addTagsLine(const StringList& list);
    /**
     * Change the value of key inside the file
     * and reload it to the local variable
     */
    void changeTitle(std::string title);
    void changePinned(bool pinned);
    void changeFavorited(bool favorited);
    void overrideTags(const StringList& list);
    inline void changeTags(const StringList& list)	{	overrideTags(list);			}
    /**
     * abstraction function to append a tag into a file
     * update the local variable after appending
     */
    bool appendTag(std::string tag);
    inline void appendTags(const StringList& tags) { for (auto& i : tags) appendTag(i); }
    /**
     * remove entirely a specific item (line) from the file's header
     */
    inline void removePinnedLine()		{	removePinnedItemFromHeader(m_path);		}
    inline void removeFavoritedLine()	{	removeFavoritedItemFromHeader(m_path);	}
    inline void removeTagsLine()		{	removeTagsItemFromHeader(m_path);		}

private:
    inline bool findReplace(const std::string& old_str, const std::string& new_str)
        { return BaseElement::replace(old_str, new_str, m_path); }
    /***
     * Reload individual items (read the file to set the local variables)
     */
    inline void reloadTitle()		{	setTitle(getTitleValueFromHeader());		}
    inline void reloadPinned()		{	setPinned(getPinnedValueFromHeader());		}
    inline void reloadFavorited()	{	setFavorited(getFavoritedValueFromHeader());}
    inline void initTags() 			{	initTags(getHeader(m_path));				}
    void initTags(const StringList& header);

    /**
     * construct the object (this) by calling all the appropriate functions
     */
    void setup(const fs::path& path);
};


#endif // Element_H
