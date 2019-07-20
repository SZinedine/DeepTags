#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "fetch.hpp"
#include "headers.hpp"
#include "extract.hpp"
#include "common.hpp"

class Element {
public:
    Element(){}
    Element(const fs::path& p, std::string& t, Tags& tg, const bool& pinned=false, const bool& fav=false)
            { m_path = p; m_title = t; m_tags = tg; m_pin = pinned; m_favorite = fav; }

    inline fs::path path() const			{	return m_path;		}
    inline std::string title() const		{	return m_title;		}
    inline Tags tags() const				{	return m_tags;		}
    inline bool pinned() const				{	return m_pin;		}
    inline bool favorited() const			{	return m_favorite;	}

    inline void setPath(const fs::path& p)	{	m_path = p;			}
    inline void setTitle(const std::string &t){	m_title = t;		}
    inline void setTags(const Tags& t)		{	m_tags = t;			}
    inline void setPinned(const bool& p)	{	m_pin = p;			}
    inline void setFavorite(const bool& f)	{	m_favorite = f;		}

    bool operator==(const Element& other) {	return path() == other.path(); }
    bool operator!=(const Element& other) {	return path() != other.path(); }

private:
    fs::path m_path;
    std::string m_title;
    Tags m_tags;
    bool m_pin = false;
    bool m_favorite = false;
};

typedef std::vector<Element*> ElementsList;

/**
 * construct a single element from the file path
 * by calling all the other functions 
 * to get the header, find its elements and parse them
 */
Element* construct_single_element(const fs::path& path);


/**
 * construct the list of Elements 
 * by iterating through the list of file paths
 * and calling to each of them the construct_single_element
 * put everything in a vector of elements
 */
ElementsList construct_list_elements(const PathsList& f);




#endif
