#ifndef FETCH_HPP
#define FETCH_HPP

#include "common.hpp"


/**
 * verify if a file as a markdown extension
 */
bool isMD(const fs::path &f);


/**
 * scans all the files of a directory and returns the mardown files inside a vector<path>
 */
PathsList fetch_files(const std::string& dir);


/** 
 * test function: fetches the list of files and print it on the terminal
 */
void print_files(const std::string& dir);

#endif
