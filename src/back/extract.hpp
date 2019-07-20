#ifndef EXTRACT_HPP
#define EXTRACT_HPP
/**
 *	extract values of YAML lines retrieved from MD files
 */

#include "yaml-cpp/yaml.h"
#include "common.hpp"

typedef std::vector< StringList > Tags;

/** receives the line of the file containing 
 * the tags and extract them 
 * then return them inside a vector of strings.
 * this is the form of the string to receive:
 *    "tags: [Notebooks/sheets/random, status/infinite, type/all]"
 */
StringList extract_tags(const std::string& t);


/**
 * Extract the title from a string in this form: "title: this is the title"
 */
std::string extract_title(const std::string& tit);


/**
 * receives : "pinned: true"
 * returns  : true
 */
bool extract_pinned(const std::string& pi);

/**
 * receives : "favorited: true"
 * returns  : true
 */
bool extract_favorited(const std::string& fav);


/**
 * split a single string into particles and return them in a vector of strings
 * for example:
 *      input: "Notebooks/sheets/random"
 *      output: {"Notebooks", "sheets, "random"}
 * 
 * use in this way: split("Notebooks/sheets/random", "/");
 * 
 * function found in: 
      https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
*/
StringList split_single_tag (const std::string& s, const std::string& delimiter="/");


/**
 * transform the raw tags retrieved from the file,
 * parse them using split_single_tag()
 * and put them inside a vector of vectors of strings (Tags typedef)
 */
Tags parse_tags(StringList& raw_tags);


/**
 * search in a list of headers (returned by getHeader) for the line "title:"
 * and return the title (using extract_title)
 */
std::string find_title_inheader(const StringList& header);

/**
 * search in a list of headers (returned by getHeader) for the line "tags:"
 * and return the title (using extract_ta)
 */
std::string find_tags_inheader(const StringList& header);

/**
 * search in a list of headers (returned by getHeader) for the line "pinned:"
 * and return the title (using extract_pinned)
 */
std::string find_pinned_inheader(const StringList& header);		////////

/**
 * search in a list of headers (returned by getHeader) for the line "favorited:"
 * and return the title (using extract_favorited)
 */
std::string find_favorite_inheader(const StringList& header);		////////

#endif
