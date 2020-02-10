#include "baseelement.h"
#include <algorithm>
#include <cctype>
#include <fstream>


std::string BaseElement::getTitle(const StringList& header) {
    std::string found = findTitle(header);
    if (found.empty()) return "";
    found = parseString(found);
    remove_quotations(found);
    return found;
}

Tags BaseElement::getParsedTags(const StringList& header) {
    StringList unparsed = getUnparsedTags(header);
    if (unparsed.empty()) return Tags{};

    Tags res;
    for (const std::string& i : unparsed) res.push_back(split(i));
    return res;
}

StringList BaseElement::getUnparsedTags(const StringList& header) {
    std::string found = findTags(header);
    if (found.empty()) return StringList();
    StringList res = parseArray(found);
    for (std::string& s : res) processTag(s);
    return res;
}

bool BaseElement::isPinned(const StringList& header) {
    std::string found = findPinned(header);
    if (found.empty()) return false;
    return parseBool(found);
}

bool BaseElement::isFavorited(const StringList& header) {
    std::string found = findFavorited(header);
    if (found.empty()) return false;
    return parseBool(found);
}

bool BaseElement::isDeleted(const StringList& header) {
    std::string found = findDeleted(header);
    if (found.empty()) return false;
    return parseBool(found);
}


std::string BaseElement::getTitle(const fs::path& path) {
    return getTitle(getHeader(path));
}

bool BaseElement::isPinned(const fs::path& path) {
    return isPinned(getHeader(path));
}

bool BaseElement::isFavorited(const fs::path& path) {
    return isFavorited(getHeader(path));
}

bool BaseElement::isDeleted(const fs::path& path) {
    return isDeleted(getHeader(path));
}


// used by all other functions that search for a key in a header
std::string BaseElement::findLine(const std::string& key, const StringList& header) {
    const std::string k = key + ":";
    for (std::string s : header) {
        trim(s);
        if (s.find(k) == 0) return s;
    }
    return std::string("");
}


void BaseElement::setTitle(const fs::path& path, const std::string& title) {
    if (!hasTitleKey(getHeader(path))) return;
    if (title.empty()) return;
    std::string t = title;
    trim(t);
    t                    = makeTitleLine(t);
    std::string old_line = findTitle(getHeader(path));
    replace(old_line, t, path);
}

void BaseElement::setPinned(const fs::path& path, const bool& pin) {
    if (!hasPinnedKey(getHeader(path))) return;
    std::string pinned = makePinnedLine(pin);
    std::string old    = findPinned(getHeader(path));
    if (old.empty()) return;
    replace(old, pinned, path);
}

void BaseElement::setFavorited(const fs::path& path, const bool& favorited) {
    if (!hasFavoritedKey(getHeader(path))) return;
    std::string fav = makeFavoritedLine(favorited);
    std::string old = findFavorited(getHeader(path));
    if (old.empty()) return;
    replace(old, fav, path);
}

void BaseElement::setDeleted(const fs::path& path, const bool& deleted) {
    if (!hasDeletedKey(getHeader(path))) return;
    std::string del = makeDeletedLine(deleted);
    std::string old = findDeleted(getHeader(path));
    if (old.empty()) return;
    replace(old, del, path);
}


std::string BaseElement::getValue(std::string line) {
    trim(line);
    std::string::size_type pos = line.find_first_of(':') + 1;
    line                       = line.substr(pos);
    trim(line);
    return line;
}


std::string BaseElement::parseString(const std::string& line) {
    return getValue(line);
}


bool BaseElement::parseBool(const std::string& line) {
    std::string val = getValue(line);
    remove_quotations(val);
    return (val == "true");
}

StringList BaseElement::parseArray(const std::string& line) {
    std::string val = getValue(line);
    unwrap(val, "[", "]");

    StringList res = split(val, ",");
    for (std::string& i : res) {
        trim(i);
        remove_quotations(i);
    }

    return res;
}


void BaseElement::unwrap(std::string& str, const std::string& before, const std::string& after) {
    if (str.empty()) return;
    auto isWrapped = [&]() {
        return ((str.find(before) == 0) && (str.rfind(after) == str.size() - 1));
    };
    if (!isWrapped()) return;
    str = str.substr(1, str.size() - 2);
}

void BaseElement::enwrap(std::string& str, const std::string& before, const std::string& after) {
    trim(str);
    str = before + str + after;
}

void BaseElement::remove_quotations(std::string& str) {
    unwrap(str, "\"", "\"");
    unwrap(str, "\'", "\'");
}

void BaseElement::createNewFile(const fs::path& p, std::string title) {
    trim(title);
    if (title.empty()) title = "untitled";
    const std::string title_line = makeTitleLine(title);

    StringList content;
    content.push_back("---");
    content.push_back(title_line);
    content.push_back("---");
    content.push_back("\n");
    content.push_back(title);
    content.push_back("==========");

    writeContentToFile(content, fs::path(p));
}


PathsList BaseElement::fetch_files(const std::string& dir) {
    PathsList list;

    for (auto& f : fs::recursive_directory_iterator(dir)) {
        fs::path p = fs::path(f);
        if (isMD(p)) list.push_back(p);
    }

    return list;
}


StringList BaseElement::getHeader(const fs::path& path) {
    StringList header;
    std::ifstream myfile(path);
    if (!myfile.is_open()) {
        std::cerr << "the following file failed to open:\n"
                  << "    " << path << "\n";
        return header;
    }

    int headerMark = 0;   // how many times "---" have been encountered before stopping (2)
    std::string line;
    std::getline(myfile, line);
    if (line == "---")
        headerMark++;   // stop if the first line doesn't start with "---"
    else
        return header;
    while (std::getline(myfile, line)) {
        if (headerMark == 2) break;   // break if we finished the header
        if (line == "---")
            headerMark++;
        else
            header.push_back(line);   // add to the vector if the current line is metadata
    }
    myfile.close();

    return header;
}


bool BaseElement::isMD(const fs::path& f) {
    std::string ext = std::string(f.extension().string());
    return (ext == ".md" || ext == ".markdown" || ext == ".MD");
}


int BaseElement::nbItemsInHeader(const fs::path& fi) {
    std::ifstream myfile(fi);
    if (!myfile.is_open())
        std::cerr << "the following file failed to open:\n"
                  << "    " << fi << "\n";
    std::string line;
    int headerItems  = 0;   // lines between the header markers
    int headerMarker = 0;
    while (std::getline(myfile, line)) {
        if (headerMarker == 2) break;
        if (headerMarker == 1) headerItems++;
        if (line == "---") headerMarker++;
    }
    myfile.close();
    return headerItems;
}


StringList BaseElement::split(const std::string& s, const std::string& delimiter) {
    std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    StringList res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token     = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}


void BaseElement::createHeader(const fs::path& file, const std::string& title) {
    if (hasHeader(file)) return;
    StringList header;
    header.push_back("---");
    header.push_back(makeTitleLine(title));
    header.push_back("---\n");

    StringList entire_file = getFileContent(file);
    for (const std::string& i : entire_file) header.push_back(i);
    writeContentToFile(header, file);
}


std::string BaseElement::makeTitleLine(std::string title) {
    title = (title.empty()) ? "untitled" : title;
    return composeStringItem("title", title);
}


std::string BaseElement::makePinnedLine(const bool& pinned) {
    return composeBoolItem("pinned", pinned);
}

std::string BaseElement::makeFavoritedLine(const bool& favorited) {
    return composeBoolItem("favorited", favorited);
}

std::string BaseElement::makeDeletedLine(const bool& del) {
    return composeBoolItem("deleted", del);
}

std::string BaseElement::makeTagsLine(const StringList& lst) {
    return composeArrayItem("tags", lst);
}


std::string BaseElement::composeStringItem(std::string key, std::string value) {
    trim(key);
    key.append(": ");
    enwrap(value, "'", "'");
    key.append(value);
    return key;
}

std::string BaseElement::composeBoolItem(std::string key, const bool& value) {
    trim(key);
    std::string v = ((value) ? "true" : "false");
    key.append(": " + v);
    return key;
}

std::string BaseElement::composeArrayItem(std::string key, const StringList& value) {
    trim(key);
    key.append(": ");
    std::string val;

    for (StringList::size_type i = 0; i < value.size(); i++) {
        val.append(trim(value[i]));
        if (i != value.size() - 1) val.append(", ");
    }

    enwrap(val, "[", "]");
    key.append(val);
    return key;
}

bool BaseElement::validTagToAdd(const std::string& tag) {
    // test if reserved tags
    if (tag.empty()) return false;
    for (const std::string& s : { "All Notes", "Notebooks", "Favorite", "Untagged", "Trash" })
        if (s == tag) {
            std::cerr << "Error. Cannot add a basic tag to a file\n";
            return false;
        }

    // forbidden characters (,/)
    auto hasChar = [tag](const std::string& c) {   // has one of the characters in the string
        for (const auto& i : c)
            if (tag.find(i) != std::string::npos) return true;
        return false;
    };
    if (hasChar(",")) return false;
    if (tag.find('/') == 0 || tag.rfind('/') == tag.size() - 1) return false;

    return true;
}


bool BaseElement::hasHeader(const fs::path& fi) {
    std::ifstream myfile(fi);
    if (!myfile.is_open())
        std::cerr << "the following file failed to open:\n"
                  << "    " << fi << "\n";

    std::string line;
    int headerItems  = 0;   // lines between the header markers
    int headerMarker = 0;
    while (std::getline(myfile, line)) {
        if (headerMarker == 1 && line != "---") headerItems++;
        if (line == "---") headerMarker++;
        if (headerMarker == 2) break;
    }
    myfile.close();
    return (headerMarker == 2 && (headerItems <= 8 && headerItems >= 1));
}


void BaseElement::addItemToHeader(const std::string& item, const fs::path& path) {
    if (!hasHeader(path)) createHeader(path, path.stem().string());

    StringList entire_file = getFileContent(path);
    StringList newFile;

    int headerIndic = 0;
    for (const std::string& line : entire_file) {
        if (line == "---") headerIndic++;
        if (headerIndic == 2) {
            newFile.push_back(item);
            headerIndic++;
        }
        newFile.push_back(line);
    }
    writeContentToFile(newFile, path);
}


void BaseElement::addPinnedItem(std::string pinnedLine, const fs::path& path) {
    if (hasPinnedKey(getHeader(path))) return;
    trim(pinnedLine);
    if (pinnedLine == "pinned: true" || pinnedLine == "pinned: false")
        addItemToHeader(pinnedLine, path);
}

void BaseElement::addFavoritedItem(std::string favoritedLine, const fs::path& path) {
    if (hasFavoritedKey(getHeader(path))) return;
    trim(favoritedLine);
    if (favoritedLine == "favorited: true" || favoritedLine == "favorited: false")
        addItemToHeader(favoritedLine, path);
}


void BaseElement::addDeletedItem(std::string deletedLine, const fs::path& path) {
    if (hasDeletedKey(getHeader(path))) return;
    trim(deletedLine);
    if (deletedLine == "deleted: true" || deletedLine == "deleted: false")
        addItemToHeader(deletedLine, path);
}


void BaseElement::addTagsItem(std::string tagsLine, const fs::path& path) {
    if (hasTagsKey(getHeader(path))) return;
    trim(tagsLine);
    if (tagsLine.size() < 9) return;
    addItemToHeader(tagsLine, path);
}


void BaseElement::removeLineFromHeader(const std::string& line, const fs::path& path) {
    if (!hasHeader(path)) return;
    if (line.empty()) return;
    StringList content = getFileContent(path);
    StringList newContent;
    for (const std::string& l : content) {
        if (l == line) continue;
        newContent.push_back(l);
    }
    writeContentToFile(newContent, path);
}


void BaseElement::removePinnedItemFromHeader(const fs::path& path) {
    if (!hasPinnedKey(getHeader(path))) return;
    std::string str = findPinned(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeFavoritedItemFromHeader(const fs::path& path) {
    if (!hasFavoritedKey(getHeader(path))) return;
    std::string str = findFavorited(getHeader(path));
    removeLineFromHeader(str, path);
}

void BaseElement::removeDeletedItemFromHeader(const fs::path& path) {
    if (!hasDeletedKey(getHeader(path))) return;
    std::string str = findDeleted(getHeader(path));
    removeLineFromHeader(str, path);
}

void BaseElement::removeTagsItemFromHeader(const fs::path& path) {
    if (!hasTagsKey(getHeader(path))) return;
    std::string str = findTags(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeTitleItemFromHeader(const fs::path& path) {
    if (!hasTitleKey(getHeader(path))) return;
    std::string str = findTitle(getHeader(path));
    removeLineFromHeader(str, path);
}

std::string& BaseElement::processTag(std::string& tag) {
    trim(tag);
    while (tag.find('/') == 0) tag = tag.substr(1);
    while (tag.rfind('/') == tag.size() - 1) tag = tag.substr(0, tag.size() - 1);
    trim(tag);
    return tag;
}


std::string BaseElement::combineTags(const StringList& chain) {
    std::string res;
    for (StringList::size_type i = 0; i < chain.size(); i++) {
        res.append(chain[i]);
        if (i != chain.size() - 1) res.append("/");
    }
    return res;
}


void BaseElement::trim(std::string& s) {
    // trim left
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));

    // trim right
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

[[nodiscard]] std::string BaseElement::trim(const std::string& s) {
    std::string res = s;
    trim(res);
    return res;
}


bool BaseElement::replace(const std::string& old_str, const std::string& new_str,
                          const fs::path& path) {
    if (old_str == new_str) return true;
    StringList entire_file = getFileContent(path);
    if (entire_file.empty()) return false;

    // replace the string in the vector
    bool found = false;
    for (std::string& s : entire_file) {
        if (s == old_str) {
            s     = new_str;
            found = true;
            break;
        }
    }
    if (!found) return false;

    writeContentToFile(entire_file, path);
    return true;
}


StringList BaseElement::getFileContent(const fs::path& file) {
    std::ifstream f(file);

    StringList entire_file;
    std::string line;
    while (std::getline(f, line)) entire_file.push_back(line);
    f.close();

    return entire_file;
}

void BaseElement::writeContentToFile(const StringList& content, const fs::path& file) {
    std::ofstream f(file);
    if (!f.is_open()) return;

    for (const std::string& i : content) f << i << "\n";
    f.close();
}
