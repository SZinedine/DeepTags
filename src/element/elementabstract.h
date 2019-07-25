#ifndef ELEMENTABSTRACT_H
#define ELEMENTABSTRACT_H

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class Element;
typedef std::vector<std::string> StringList;
typedef std::vector<fs::path> PathsList;
typedef std::vector<Element*> ElementsList;
typedef std::vector<std::vector<std::string>> Tags;

class ElementAbstract
{
public:
    ElementAbstract();
    ElementAbstract(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned=false, const bool& favorited=false);
    ElementAbstract(const ElementAbstract& other);
    ~ElementAbstract(){}

    // accessers
    inline fs::path path() 		const	{	return m_path;		}
    inline std::string title() 	const	{	return m_title;		}
    inline Tags tags() 			const	{	return m_tags;		}
    inline bool pinned()		const	{	return m_pinned;	}
    inline bool favorited()		const	{	return m_favorited;	}
    // modifiers
    inline void setPath(const fs::path& path)		{	m_path = path;		}
    inline void setTitle(const std::string& title)	{	m_title = title;	}
    inline void setTags(const Tags &tags)			{	m_tags = tags;		}
    inline void setPinned(const bool &pinned)		{	m_pinned = pinned;	}
    inline void setFavorited(const bool& favorited)	{	m_favorited = favorited;}

private:
    void setup(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned=false, const bool& favorited=false);

protected:
    fs::path m_path;
    std::string m_title;
    bool m_pinned;
    bool m_favorited;
    Tags m_tags;
};

#endif // ELEMENTABSTRACT_H
