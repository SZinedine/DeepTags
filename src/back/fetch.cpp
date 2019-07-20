#include "fetch.h"

bool isMD(const fs::path &f) {
    std::string ext = std::string(f.extension());
    return ( ext == ".md" || ext == ".markdown" || ext == ".MD" );
}

PathsList fetch_files(const std::string& dir) {
    PathsList list;

    for (auto& f : fs::recursive_directory_iterator( dir )) {
        fs::path p = fs::path(f);
        if ( isMD(p) )
            list.push_back( p );
    }

    return list;
}


void print_files(const std::string& dir) {
    PathsList f = fetch_files(dir);
    for (auto &i : f) 
        std::cout << i << std::endl;
}
