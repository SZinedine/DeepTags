#include "element.h"

Element::Element(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned, const bool& favorited)
    : BaseElement(path, title, tags, pinned, favorited)
{}

Element::Element(const fs::path& path) :BaseElement() {	setup(path); }

Element::Element(const Element& other)
    : BaseElement()
{
    m_path = other.m_path;
    m_title = other.m_title;
    m_tags = other.m_tags;
    m_pinned = other.m_pinned;
    m_favorited = other.m_favorited;
}

void Element::setup(const fs::path& path) {
    m_path = path;
    const StringList header = getHeader(path);  // the complete header of the file
    setTitle( extract_title( find_title_inheader(header) ) );
    initTags(header);
    setPinned( extract_pinned( find_pinned_inheader(header) ) );
    setFavorited( extract_favorited( find_favorite_inheader(header) ) );
}


ElementsList Element::construct_list_elements(const PathsList& f) {
    ElementsList elems;
    for (const fs::path& p : f) {
        if (!isMD(p)) continue;
        elems.push_back( new Element(p) );
    }

    return elems;
}


void Element::initTags(const StringList& header) {
    StringList tags = extract_tags( find_tags_inheader(header) );
    setTags(parse_tags(tags));
}



bool Element::appendTag(std::string tag) {
    if (!validTagToAdd(tag)) return false;

    const StringList header = getHeader(m_path);
    const std::string raw_tag_header = find_tags_inheader(header);

    StringList tags = extract_tags(raw_tag_header);

    for (const std::string& i : tags)	// verify if the file doesn't contain the tag
        if (tag == i) return false;
    tags.push_back(tag);

    const std::string res = makeTagsLine(tags);
    bool ret = findReplace(raw_tag_header, res);	// write the changes into the file

    initTags();		// reload the tags into the local variable

    return ret;
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
    const StringList header = getHeader(m_path);
    std::string old = find_tags_inheader(header);
    StringList valid;
    for (const std::string& i : list)
        if (validTagToAdd(i)) valid.push_back(i);
    const std::string newTag = makeTagsLine(valid);

    findReplace(old, newTag);
    initTags();
}



void Element::addPinnedLine(const bool& val) {
    std::string res = makePinnedLine(val);
    addPinnedItem(res, m_path);
    setPinned(val);
}

void Element::addFavoritedLine(const bool& val) {
    std::string res = makeFavoritedLine(val);
    addFavoritedItem(res, m_path);
    setFavorited(val);
}


void Element::addTagsLine(const StringList& list) {
    std::string line = makeTagsLine(list);
    addTagsItem(line, m_path);
    initTags();
}


void Element::changeTitle(std::string title) {
    changeTitleInFile(title, m_path);
    reloadTitle();
}

void Element::changePinned(bool pinned) {
    if (!hasPinnedLine()) {
        addPinnedLine(pinned);
        return;
    }
    changePinnedInFile(pinned, m_path);
    reloadPinned();
}

void Element::changeFavorited(bool favorited) {
    if (!hasFavoritedLine()) {
        addFavoritedLine(favorited);
        return;
    }
    changeFavoritedInFile(favorited, m_path);
    reloadFavorited();
}


