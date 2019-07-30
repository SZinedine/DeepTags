#include "baseelement.h"
#include <fstream>
#include <algorithm>
#include <cctype>

BaseElement::BaseElement() {
    m_path = "";
    m_title = "";
    m_tags = {};
    m_pinned = false;
    m_favorited = false;

}

BaseElement::BaseElement(const fs::path& path, const std::string& title, const Tags& tags, const bool& pinned, const bool& favorited) {
    setup(path, title, tags, pinned, favorited);
}

BaseElement::BaseElement(const BaseElement& other) {
    m_path = other.m_path;
    m_title = other.m_title;
    m_tags = other.m_tags;
    m_pinned = other.m_pinned;
    m_favorited = other.m_favorited;
}



void BaseElement::setup(const fs::path& path, const std::string& title, const Tags& tags, const bool& pinned, const bool& favorited) {
    setPath(path);
    setTitle(title);
    setTags(tags);
    setPinned(pinned);
    setFavorited(favorited);
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




StringList BaseElement::getHeader(const fs::path& fi) {
    StringList header;
    std::ifstream myfile(fi);
    if (!myfile.is_open())
        std::cerr
            <<  "the following file failed to open:\n"
            << "    " << fi << "\n";

    int headerMark = 0;		// how many times "---" have been encountered before stopping (2)
    std::string line;
    while ( std::getline(myfile, line) ) {
        if (headerMark == 2) break;         // break if we finished the header
        if (line == "---") headerMark++;
        else header.push_back(line);        // add to the vector if the current line is metadata
    }
    myfile.close();

    return header;
}



bool BaseElement::isMD(const fs::path &f) {
    std::string ext = std::string(f.extension().string());
    return ( ext == ".md" || ext == ".markdown" || ext == ".MD" );
}


PathsList BaseElement::fetch_files(const std::string& dir) {
    PathsList list;

    for (auto& f : fs::recursive_directory_iterator( dir )) {
        fs::path p = fs::path(f);
        if ( isMD(p) )
            list.push_back( p );
    }

    return list;
}






std::string BaseElement::extract_title(const std::string& tit) {
    if (tit.size() < 6) return std::string("");

    std::string s = tit;
    trim(s);
    s = s.substr(7);

    remove_quotations(s);
    return s;
}

bool BaseElement::extract_pinned(const std::string& pi) {
    if (pi.empty()) return false;
    std::string res = pi;
    trim(res);
    if (res.size() < 11) return false;
    res = res.substr(8, res.size()-1);
    trim(res);

    return (res == "true");
}

bool BaseElement::extract_favorited(const std::string& fav) {
    if (fav.empty()) return false;
    std::string res = fav;
    trim(res);
    if (res.size() < 14) return false;
    res = res.substr(11);
    trim(res);

    return (res == "true");
}



StringList BaseElement::extract_tags(const std::string& s) {
    if (s.size() < 7) return StringList();
    std::string raw = s;
    trim(raw);
    raw = raw.substr(7);	// 7 is supposed to be '['
    raw = raw.substr(0, raw.size()-1);

    StringList res = split_single_tag(raw, ",");
    for (std::string& i : res) remove_quotations(i);

    return res;
}



StringList BaseElement::split_single_tag(const std::string& s, const std::string& delimiter) {
    std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    StringList res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


Tags BaseElement::parse_tags(const StringList& raw_tags) {
    Tags res;
    for (const std::string& i : raw_tags)
        res.push_back( split_single_tag(i) );
    return res;
}






std::string BaseElement::find_title_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if(s.substr(0, 6) == "title:") return s;
    }
    return "";
}


std::string BaseElement::find_tags_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if (s.substr(0, 5) == "tags:")
                return s;
    }
    return "";
}


std::string BaseElement::find_pinned_inheader(const StringList& header) {
    // pinned:
    for (const std::string& s : header) {
        if (s.length() > 7)
            if (s.substr(0, 7) == "pinned:")
                return s;
    }
    return "";
}


std::string BaseElement::find_favorite_inheader(const StringList& header) {
    // favorited:
    for (const std::string& s : header) {
        if (s.length() > 10)
            if (s.substr(0, 10) == "favorited:")
                return s;
    }
    return "";
}






void BaseElement::createHeader(const fs::path& file, const std::string& title) {
    if (hasHeader(file)) return;
    StringList header;
    header.push_back("---");
    header.push_back(makeTitleLine(title));
    header.push_back("---");

    StringList entire_file = getFileContent(file);
    for (const std::string& i : entire_file)
        header.push_back(i);
    writeContentToFile(header, file);
}


std::string BaseElement::makeTitleLine(std::string title) {
    trim(title);
    if (title.empty()) return "title: untitled";
    return "title: " + title;
}


std::string BaseElement::makePinnedLine(const bool& pinned) {
    if (pinned) return "pinned: true";
    return "pinned: false";
}


std::string BaseElement::makeFavoritedLine(const bool& favorited) {
    if (favorited) return "favorited: true";
    return "favorited: false";
}

std::string BaseElement::makeTagsLine(const StringList& lst) {
    std::string str = "tags: [";
    for (StringList::size_type i = 0 ; i < lst.size() ; i++) {
        str.append(lst[i]);
        if (i != lst.size()-1) str.append(", ");
    }
    str.append("]");
    return str;
}






bool BaseElement::hasTagItem(const fs::path& f) {
    if (!fs::exists(f)) return false;
    if (!hasHeader(f)) return false;

    return !(find_tags_inheader( getHeader(f) ).empty());
}


bool BaseElement::hasTitleItem(const fs::path& f) {
    if (!fs::exists(f)) return false;
    if (!hasHeader(f)) return false;

    return !(find_title_inheader( getHeader(f) ).empty());
}

bool BaseElement::hasPinnedItem(const fs::path& f) {
    if (!fs::exists(f)) return false;
    if (!hasHeader(f)) return false;

    return !(find_pinned_inheader( getHeader(f) ).empty());
}

bool BaseElement::hasFavoritedItem(const fs::path& f) {
    if (!fs::exists(f)) return false;
    if (!hasHeader(f)) return false;

    return !(find_favorite_inheader( getHeader(f) ).empty());
}



bool BaseElement::validTagToAdd(const std::string& tag) {
    // test if reserved tags
    for (const std::string& s : {"All Notes", "Notebooks", "Favorite", "Untagged"})
        if (s == tag) {
            std::cerr << "Error. Cannot add a basic tag to a file\n";
            return false;
        }

    // forbidden characters (,/)
    auto hasChar = [tag](const std::string c){		// has one of the characters in the string
        for (const auto& i : c)
            if (tag.find(i) != std::string::npos) return true;
        return false;
    };

    if (hasChar(",")) return false;

    return true;
}



bool BaseElement::hasHeader(const fs::path& fi) {
    std::ifstream myfile(fi);
    if (!myfile.is_open())
        std::cerr
            << "the following file failed to open:\n"
            << "    " << fi << "\n";

    std::string line;
    int headerItems = 0;	// lines between the header markers
    int headerMarker = 0;
    while (std::getline(myfile, line)) {
        if (headerMarker == 1 && line != "---") 	headerItems++;
        if (line == "---")	    headerMarker++;
        if (headerMarker == 2) 	break;
    }
    myfile.close();
    return (headerMarker == 2 && (headerItems <= 8 && headerItems >= 1));
}


int BaseElement::nbItemsInHeader(const fs::path& fi) {
    std::ifstream myfile(fi);
    if (!myfile.is_open())
        std::cerr
            << "the following file failed to open:\n"
            << "    " << fi << "\n";
    std::string line;
    int headerItems = 0;	// lines between the header markers
    int headerMarker = 0;
    while (std::getline(myfile, line)) {
        if (headerMarker == 2) 	break;
        if (headerMarker == 1) 	headerItems++;
        if (line == "---")	    headerMarker++;
    }
    myfile.close();
    return headerItems;
}







void BaseElement::addItemToHeader(const std::string& item, const fs::path& path) {
    if (!hasHeader(path)) return;

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
    if (hasPinnedItem(path)) return;
    trim(pinnedLine);
    if (pinnedLine == "pinned: true" || pinnedLine == "pinned: false")
        addItemToHeader(pinnedLine, path);
}

void BaseElement::addFavoritedItem(std::string favoritedLine, const fs::path &path) {
    if (hasFavoritedItem(path)) return;
    trim(favoritedLine);
    if (favoritedLine == "favorited: true" || favoritedLine == "favorited: false")
        addItemToHeader(favoritedLine, path);
}


void BaseElement::addTagsItem(std::string tagsLine, const fs::path& path) {
    if (hasTagItem(path)) return;
    trim(tagsLine);
    if (tagsLine.size() < 9) return;
    addItemToHeader(tagsLine, path);
}






void BaseElement::changeTitleInFile(std::string title, const fs::path& path) {
    if (!hasTitleItem(path)) return;
    if (title.empty()) return;
    trim(title);
    title = makeTitleLine(title);
    std::string old_line = find_title_inheader(getHeader(path));
    replace(old_line, title, path);
}


void BaseElement::changePinnedInFile(const bool& pin, const fs::path path) {
    if (!hasPinnedItem(path)) return;
    std::string pinned = makePinnedLine(pin);
    std::string old = find_pinned_inheader(getHeader(path));
    if (old.empty()) return;
    replace(old, pinned, path);
}


void BaseElement::changeFavoritedInFile(const bool& favorited, const fs::path path) {
    if (!hasFavoritedItem(path)) return;
    std::string fav = makeFavoritedLine(favorited);
    std::string old = find_favorite_inheader(getHeader(path));
    if (old.empty()) return;
    replace(old, fav, path);
}






void BaseElement::removeLineFromHeader(const std::string &line, const fs::path path) {
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


void BaseElement::removePinnedItemFromHeader(const fs::path &path) {
    if (!hasPinnedItem(path)) return;
    std::string str = find_pinned_inheader(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeFavoritedItemFromHeader(const fs::path &path) {
    if (!hasFavoritedItem(path)) return;
    std::string str = find_favorite_inheader(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeTagsItemFromHeader(const fs::path &path) {
    if (!hasTagItem(path)) return;
    std::string str = find_tags_inheader(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeTitleItemFromHeader(const fs::path &path) {
    if (!hasTitleItem(path)) return;
    std::string str = find_title_inheader(getHeader(path));
    removeLineFromHeader(str, path);
}






StringList BaseElement::getFileContent(const fs::path file) {
    std::ifstream f(file);

    StringList entire_file;
    std::string line;
    while (std::getline(f, line))
        entire_file.push_back(line);
    f.close();

    return entire_file;
}

void BaseElement::writeContentToFile(const StringList& content, const fs::path file) {
    std::ofstream f(file);
    if (!f.is_open()) return;

    for (const std::string& i : content)
        f << i << "\n";
    f.close();
}



std::string BaseElement::combineTags(const StringList& chain) {
    std::string res;
    for (StringList::size_type i = 0 ; i < chain.size() ; i++) {
        res.append(chain[i]);
        if (i != chain.size()-1) res.append("/");
    }
    return res;
}


bool BaseElement::replace(const std::string& old_str, const std::string& new_str, const fs::path& path) {
    if (old_str == new_str) return true;
    StringList entire_file = getFileContent(path);
    if (entire_file.empty()) return false;

    // replace the string in the vector
    bool found = false;
    for (std::string& s : entire_file) {
        if (s == old_str) {
            s = new_str;
            found = true;
            break;
        }
    }
    if (!found) return false;

    writeContentToFile(entire_file, path);
    return true;
}




 void BaseElement::trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


void BaseElement::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](int ch) {
            return !std::isspace(ch);
        }
    ));
}


void BaseElement::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}



void BaseElement::remove_quotations(std::string& str) {
    trim(str);
    char& back = str.back();
    char& front = str.front();
    char a = '\'';
    char b = '\"';
    if ( (back == a && front == a) || (back == b && front == b) ) {
        back = ' ';
        front = ' ';
        trim(str);
    }
};
