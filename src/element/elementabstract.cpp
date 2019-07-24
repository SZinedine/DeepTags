#include "elementabstract.h"

ElementAbstract::ElementAbstract() {
    m_path = "";
    m_title = "";
    m_tags = {};
    m_pinned = false;
    m_favorited = false;

}

ElementAbstract::ElementAbstract(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned, const bool& favorited) {
    setup(path, title, tags, pinned, favorited);
}


void ElementAbstract::setup(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned, const bool& favorited) {
    setPath(path);
    setTitle(title);
    setTags(tags);
    setPinned(pinned);
    setFavorited(favorited);
}

ElementAbstract::ElementAbstract(const ElementAbstract& other) {
    m_path = other.m_path;
    m_title = other.m_title;
    m_tags = other.m_tags;
    m_pinned = other.m_pinned;
    m_favorited = other.m_favorited;
}




