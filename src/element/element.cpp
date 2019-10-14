#include "element.h"

Element::Element(const fs::path& path, const std::string& title, 
        const Tags& tags, const bool& pinned, 
        const bool& favorited, const bool& deleted)
    : m_path(path), m_title(title),
      m_tags(tags), m_pinned(pinned),
      m_favorited(favorited), m_deleted(deleted)
{}

Element::Element(const fs::path& path)
{
    setup(path);
}

Element::Element(const Element& other)
{
    m_path = other.m_path;
    m_title = other.m_title;
    m_tags = other.m_tags;
    m_pinned = other.m_pinned;
    m_favorited = other.m_favorited;
    m_deleted = other.m_deleted;
}

void Element::setup(const fs::path& path) {
    m_path = path;
    const StringList header = be::getHeader(path);  // the complete header of the file
    setTitle( be::getTitle(header));
    initTags(header);
    setPinned( be::isPinned(header));
    setFavorited( be::isFavorited(header));
    setDeleted(be::isDeleted(header));
}

ElementsList Element::construct_list_elements(const PathsList& f) {
    ElementsList elems;
    for (const fs::path& p : f) {
        if (!be::isMD(p)) continue;
        elems.push_back( new Element(p) );
    }
    return elems;
}




void Element::addPinnedLine(const bool& val) {
    std::string res = be::makePinnedLine(val);
    be::addPinnedItem(res, m_path);
    setPinned(val);
    
}

void Element::addFavoritedLine(const bool& val) {
    std::string res = be::makeFavoritedLine(val);
    be::addFavoritedItem(res, m_path);
    setFavorited(val);
}

void Element::addDeletedLine(const bool& val) {
    std::string res = be::makeDeletedLine(val);
    be::addDeletedItem(res, m_path);
    setDeleted(val);
}


void Element::addTagsLine(const StringList& list) {
    std::string line = be::makeTagsLine(list);
    be::addTagsItem(line, m_path);
    initTags();
}





void Element::changeTitle(std::string title) {
    be::setTitle(m_path, title);
    setTitle(title);
}

void Element::changePinned(bool pinned) {
    setPinned(pinned);
    if (!hasPinnedLine() && pinned) {
        addPinnedLine(pinned);
        return;
    }
    if (!pinned) {
        removePinnedLine();
        return;
    }
    be::setPinned(m_path, pinned);
    setPinned(pinned);
}

void Element::changeFavorited(bool favorited) {
    setFavorited(favorited);
    if (!hasFavoritedLine() && favorited) {
        addFavoritedLine(favorited);
        return;
    }
    if (!favorited) {
        removeFavoritedLine();
        return;
    }
    be::setFavorited(m_path, favorited);
    setFavorited(favorited);
}



void Element::changeDeleted(bool deleted) {
    setDeleted(deleted);
    if (!hasDeletedLine() && deleted) {
        addDeletedLine(deleted);
        return;
    }
    if (!deleted) {
        removeDeletedLine();
        return;
    }
    be::setDeleted(m_path, deleted);
    setDeleted(deleted);
}


void Element::overrideTags(const StringList& list) {
    if (list.empty()) {		// if the list is empty, remove the tag item from the file
        removeTagsLine();
        return;
    }
    if (!hasTagsLine()) {		// if the file doesn't have a tag item, create it
        addTagsLine(list);
        return;
    }
    std::string old = be::findTags( be::getHeader(m_path) );        // modified
    StringList valid;
    for (std::string i : list) {
        be::trim(i);
        if (be::validTagToAdd(i)) valid.push_back(i);
    }
    const std::string newTag = be::makeTagsLine(valid);

    findReplace(old, newTag);
    initTags();
}


bool Element::appendTag(std::string tag) {
    be::trim(tag);
    if (!be::validTagToAdd(tag)) return false;
    if (!hasTagsLine()) {
        addTagsLine(StringList{tag});
        return true;
    }

    const StringList header = be::getHeader(m_path);
    const std::string raw_tag_header = be::findTags(header);

    StringList tags = be::getUnparsedTags(header);

    for (const std::string& i : tags)	// verify if the file doesn't contain the tag
        if (tag == i) return false;
    tags.push_back(tag);

    const std::string res = be::makeTagsLine(tags);
    bool ret = findReplace(raw_tag_header, res);	// write the changes into the file

    initTags();		// reload the tags into the local variable

    return ret;
}





void Element::removePinnedLine() {
    be::removePinnedItemFromHeader(m_path);
    setPinned(false);
}

void Element::removeFavoritedLine() {
    be::removeFavoritedItemFromHeader(m_path);
    setFavorited(false);
}

void Element::removeDeletedLine() {
    be::removeDeletedItemFromHeader(m_path);
    setDeleted(false);
}

void Element::removeTagsLine() {
    be::removeTagsItemFromHeader(m_path);
    setTags({{}});
}



void Element::initTags(const StringList& header) {
    setTags(be::getParsedTags(header));
}


