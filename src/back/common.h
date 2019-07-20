#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

typedef std::vector<std::string> StringList;

/**
 * store the .md files found in a provided directory
 */
typedef std::vector<fs::path> PathsList;


#endif
