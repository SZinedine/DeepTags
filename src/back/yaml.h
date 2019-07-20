#include <iostream>
#include <locale>
#include <cctype>
#include <algorithm>
#include "common.h"


/**
 * this replaces the usage of yaml-cpp
 */

std::string extract_title_(const std::string& tit);


bool extract_pinned_(const std::string& pi);



bool extract_favorited_(const std::string& fav);


StringList extract_tags_(const std::string& s);

/**
 * Some lines are surrounded with quotations,
 * call this function on every title to remove them
 */
void remove_quotations(std::string& str);


/**
 * found in: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 */
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
        [](int ch) {
            return !std::isspace(ch);
        }
    ));
}


inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}




