#include "yaml.h"
#include "extract.h"
#include <cstring>

std::string extract_title_(const std::string& tit) {
    if (tit.size() < 6) return std::string("");

    std::string s = tit;
    trim(s);
    s = s.substr(7);

    remove_quotations(s);
    return s;
}

bool extract_pinned_(const std::string& pi) {
    if (pi.empty()) return false;
    std::string res = pi;
    trim(res);
    if (res.size() < 11) return false;
    res = res.substr(8, res.size()-1);
    trim(res);
    
    return (res == "true");
}


bool extract_favorited_(const std::string& fav) {
    if (fav.empty()) return false;
    std::string res = fav;
    trim(res);
    if (res.size() < 14) return false;
    res = res.substr(11);
    trim(res);

    return (res == "true");
}


StringList extract_tags_(const std::string& s) {
    if (s.size() < 7) return StringList();
    std::string raw = s;
    trim(raw);
    raw = raw.substr(7);
    raw = raw.substr(0, raw.size()-1);

    StringList res = split_single_tag(raw, ",");
    for (std::string& i : res) remove_quotations(i);

    return res;
}


void remove_quotations(std::string& str) {
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

























