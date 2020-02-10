#include "element.h"

Element::Element(const fs::path& path) : m_path(path) {
    setup();
}

Element::Element(const Element& other) {
    m_path      = other.m_path;
    m_title     = other.m_title;
    m_tags      = other.m_tags;
    m_pinned    = other.m_pinned;
    m_favorited = other.m_favorited;
    m_deleted   = other.m_deleted;
    m_header    = other.m_header;
}

void Element::setup() {
    m_header = be::getHeader(m_path);
    if (m_header.empty()) {
        setTitle(m_path.stem().string());
        setPinned(false);
        setFavorited(false);
        setDeleted(false);
        return;
    }
    setTitle(be::getTitle(m_header));
    loadTags(m_header);
    setPinned(be::isPinned(m_header));
    setFavorited(be::isFavorited(m_header));
    setDeleted(be::isDeleted(m_header));
}

ElementsList Element::constructElementList(const PathsList& f) {
    ElementsList elems;
    for (const fs::path& p : f) {
        if (!be::isMD(p)) continue;
        elems.push_back(new Element(p));
    }
    return elems;
}


void Element::addPinnedLine(const bool& val) {
    std::string res = be::makePinnedLine(val);
    be::addPinnedItem(res, m_path);
    setPinned(val);
    reloadHeader();
}

void Element::addFavoritedLine(const bool& val) {
    std::string res = be::makeFavoritedLine(val);
    be::addFavoritedItem(res, m_path);
    setFavorited(val);
    reloadHeader();
}

void Element::addDeletedLine(const bool& val) {
    std::string res = be::makeDeletedLine(val);
    be::addDeletedItem(res, m_path);
    setDeleted(val);
    reloadHeader();
}


void Element::addTagsLine(const StringList& list) {
    std::string line = be::makeTagsLine(list);
    be::addTagsItem(line, m_path);
    loadTags();
}


void Element::changeTitle(const std::string& title) {
    be::setTitle(m_path, title);
    setTitle(title);
    reloadHeader();
}

void Element::changePinned(bool pinned) {
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
    reloadHeader();
}

void Element::changeFavorited(bool favorited) {
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
    reloadHeader();
}


void Element::changeDeleted(bool deleted) {
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
    reloadHeader();
}


void Element::overrideTags(const StringList& list) {
    if (list.empty()) {   // if the list is empty, remove the tag item from the file
        removeTagsLine();
        return;
    }
    if (!hasTagsLine()) {   // if the file doesn't have a tag item, create it
        addTagsLine(list);
        return;
    }
    std::string old = be::findTags(m_header);
    StringList valid;
    for (std::string i : list) {
        be::processTag(i);
        if (be::validTagToAdd(i))
            valid.push_back(i);
        else
            std::cerr << "'" << i << "' isn not a valid tag\n";
    }
    const std::string newTag = be::makeTagsLine(valid);

    be::replace(old, newTag, m_path);
    loadTags();
}


bool Element::appendTag(std::string tag) {
    be::processTag(tag);
    if (!be::validTagToAdd(tag)) return false;
    if (!hasTagsLine()) {
        addTagsLine(StringList{ tag });
        return true;
    }

    const std::string raw_tag_header = be::findTags(m_header);
    StringList tags                  = be::getUnparsedTags(m_header);

    for (const std::string& i : tags)   // verify if the file doesn't contain the tag
        if (tag == i) return false;
    tags.push_back(tag);

    const std::string res = be::makeTagsLine(tags);
    bool ret = be::replace(raw_tag_header, res, m_path);   // write the changes into the file

    loadTags();

    return ret;
}


void Element::removePinnedLine() {
    be::removePinnedItemFromHeader(m_path);
    setPinned(false);
    reloadHeader();
}

void Element::removeFavoritedLine() {
    be::removeFavoritedItemFromHeader(m_path);
    setFavorited(false);
    reloadHeader();
}

void Element::removeDeletedLine() {
    be::removeDeletedItemFromHeader(m_path);
    setDeleted(false);
    reloadHeader();
}

void Element::removeTagsLine() {
    be::removeTagsItemFromHeader(m_path);
    setTags(Tags());
    reloadHeader();
}


void Element::loadTags(const StringList& header) {
    setTags(be::getParsedTags(header));
    reloadHeader();
}
