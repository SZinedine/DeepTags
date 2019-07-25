#include "element.h"
#include <fstream>
#include <algorithm>

Element::Element(const fs::path& path, const std::string& title, const Tags tags, const bool& pinned, const bool& favorited)
    : ElementAbstract(path, title, tags, pinned, favorited)
{}

Element::Element(const fs::path& path) :ElementAbstract() {	setup(path); }

Element::Element(const Element& other)
    : ElementAbstract()
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
    StringList tags_list = extract_tags( find_tags_inheader(header) );
    setTags( parse_tags(tags_list) );
    setPinned( extract_pinned( find_pinned_inheader(header) ) );
    setFavorited( extract_favorited( find_favorite_inheader(header) ) );
}


ElementsList Element::construct_list_elements(const PathsList& f) {
    ElementsList elems;
    for (const fs::path& p : f) 
        elems.push_back( new Element(p) );

    return elems;
}


bool Element::hasHeader(const fs::path& fi) {
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
    if (headerMarker == 2 && (headerItems < 7 && headerItems >= 1))
        return true;
    else return false;
}


int Element::nbItemsInHeader(const fs::path& fi) {
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


StringList Element::getHeader(const fs::path fi) {
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


bool Element::isMD(const fs::path &f) {
    std::string ext = std::string(f.extension());
    return ( ext == ".md" || ext == ".markdown" || ext == ".MD" );
}

PathsList Element::fetch_files(const std::string& dir) {
    PathsList list;

    for (auto& f : fs::recursive_directory_iterator( dir )) {
        fs::path p = fs::path(f);
        if ( isMD(p) )
            list.push_back( p );
    }

    return list;
}


std::string Element::extract_title(const std::string& tit) {
    if (tit.size() < 6) return std::string("");

    std::string s = tit;
    trim(s);
    s = s.substr(7);

    remove_quotations(s);
    return s;
}

bool Element::extract_pinned(const std::string& pi) {
    if (pi.empty()) return false;
    std::string res = pi;
    trim(res);
    if (res.size() < 11) return false;
    res = res.substr(8, res.size()-1);
    trim(res);

    return (res == "true");
}

bool Element::extract_favorited(const std::string& fav) {
    if (fav.empty()) return false;
    std::string res = fav;
    trim(res);
    if (res.size() < 14) return false;
    res = res.substr(11);
    trim(res);

    return (res == "true");
}



StringList Element::extract_tags(const std::string& s) {
    if (s.size() < 7) return StringList();
    std::string raw = s;
    trim(raw);
    raw = raw.substr(7);	// 7 is supposed to be '['
    raw = raw.substr(0, raw.size()-1);

    StringList res = split_single_tag(raw, ",");
    for (std::string& i : res) remove_quotations(i);

    return res;
}


void Element::remove_quotations(std::string& str) {
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


StringList Element::split_single_tag(const std::string& s, const std::string& delimiter) {
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


std::string Element::find_title_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if(s.substr(0, 6) == "title:") return s;
    }
    return "";
}


std::string Element::find_tags_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if (s.substr(0, 5) == "tags:")
                return s;
    }
    return "";
}


std::string Element::find_pinned_inheader(const StringList& header) {
    // pinned:
    for (const std::string& s : header) {
        if (s.length() > 7)
            if (s.substr(0, 7) == "pinned:")
                return s;
    }
    return "";
}


std::string Element::find_favorite_inheader(const StringList& header) {
    // favorited:
    for (const std::string& s : header) {
        if (s.length() > 10)
            if (s.substr(0, 10) == "favorited:")
                return s;
    }
    return "";
}


Tags Element::parse_tags(StringList& raw_tags) {
    Tags res;
    for (const std::string& i : raw_tags)
        res.push_back( split_single_tag(i) );
    return res;
}


void Element::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](int ch) {
            return !std::isspace(ch);
        }
    ));
}


void Element::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

 void Element::trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


std::string Element::combineTags(const StringList& chain) {
    std::string res = "";
    for (StringList::size_type i = 0 ; i < chain.size() ; i++) {
        res.append(chain[i]);
        if (i != chain.size()-1) res.append("/");
    }
    return res;
}

/**
 * extract file's tags
 * append the new tag into the list
 * get the file's text.
 *
 */
bool Element::appendTag(const std::string &tag) {
    if (!validTagToAdd(tag)) return false;
    auto arrayToTagString = [](const StringList& lst){
        std::string str = "tags: [";
        for (StringList::size_type i = 0 ; i < lst.size() ; i++) {
            str.append(lst[i]);
            if (i != lst.size()-1) str.append(", ");
        }
        str.append("]");
        return str;
    };

    const StringList header = getHeader(m_path);
    const std::string raw_tag_header = find_tags_inheader(header);

    StringList tags = extract_tags(raw_tag_header);

    for (const std::string& i : tags)	// verify if the file doesn't contain the tag
        if (tag == i) return false;
    tags.push_back(tag);

    const std::string res = arrayToTagString(tags);
    bool ret = findReplace(raw_tag_header, res);	// write the changes into the file

    reload();
    return ret;
}


bool Element::findReplace(const std::string& old_str, const std::string& new_str) {
    StringList entire_file;
    std::ifstream file(m_path);
    if (!file.is_open()) return false;

    // copy the file line by line to a vector
    std::string line;
    while (std::getline(file, line))
        entire_file.push_back(line);
    file.close();

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

    // write the result
    std::ofstream f(m_path);
    if (!f.is_open()) return false;

    for (std::string& i : entire_file) {
        f << i << "\n";
    }

    return true;
}



bool Element::validTagToAdd(const std::string& tag) {
    if (tag.size() < 9) return true;
    StringList lst = {"All Notes", "Notebooks", "Favorite", "Untagged"};
    for (const std::string& s : lst)
        if (s == tag) {
            std::cerr << "Error. Cannot add a basic tag to a file\n";
            return false;
        }
    return true;
}
























