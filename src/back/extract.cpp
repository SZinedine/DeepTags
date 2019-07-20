#include "extract.hpp"

StringList extract_tags(const std::string& t) {
    YAML::Node node = YAML::Load(t);
    auto tag = node["tags"];
    StringList list;
    for (auto i : tag) 
        list.push_back( i.as<std::string>() );

    return list;
}

std::string extract_title(const std::string& tit) {
    YAML::Node node = YAML::Load(tit);
    return node["title"].as<std::string>();
}

StringList split_single_tag (const std::string& s, const std::string& delimiter) {
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

bool extract_pinned(const std::string& pi) {
    if (pi.empty()) return false;
    YAML::Node node = YAML::Load(pi);
    return node["pinned"].as<bool>();
}

bool extract_favorited(const std::string& fav) {
    if (fav.empty()) return false;
    YAML::Node node = YAML::Load(fav);
    return node["favorited"].as<bool>();
}

std::string find_title_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if(s.substr(0, 6) == "title:") return s;
    }
    return "";
}

std::string find_tags_inheader(const StringList& header) {
    for (const std::string& s : header) {
        if(s.length() > 5)
            if (s.substr(0, 5) == "tags:")
                return s;
    }
    return "";
}

std::string find_pinned_inheader(const StringList& header) {
    // pinned:
    for (const std::string& s : header) {
        if (s.length() > 7)
            if (s.substr(0, 7) == "pinned:")
                return s;
    }
    return "";
}

std::string find_favorite_inheader(const StringList& header) {
    // favorited:
    for (const std::string& s : header) {
        if (s.length() > 10)
            if (s.substr(0, 10) == "favorited:")
                return s;
    }
    return "";
}

Tags parse_tags(StringList& raw_tags) {
    Tags res;
    for (const std::string& i : raw_tags)
        res.push_back( split_single_tag(i) );
    return res;
}

