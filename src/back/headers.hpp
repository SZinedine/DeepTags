#ifndef HEADERS_HPP
#define HEADERS_HPP

/**
 *	verify if a file as a header and return the header of the file in a list of strings
 */

#include "common.hpp"


/**
 * verify if an md file has a header by checking if its first line is "---"
 */
bool hasHeader(const fs::path& fi);


/**
 * get the header of an md file: all the text 
 * that is encompassed between 2 lines that contains 3 dashes (---)
 */
StringList getHeader(const fs::path fi);

#endif
