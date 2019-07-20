#include <fstream>
#include "headers.hpp"


bool hasHeader(const fs::path& fi) {
    std::ifstream myfile(fi);
    if (!myfile.is_open()) {
        std::cerr 
            << "the following file failed to open:\n" 
            << "    " << fi << "\n";
    }
    std::string line;
    std::getline(myfile, line);
    myfile.close();
    std::cout << "'" << line << "'" << std::endl;
    return (line == "---");
}

StringList getHeader(const fs::path fi) {
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
