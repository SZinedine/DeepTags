#include "element.hpp"

Element* construct_single_element(const fs::path& path) {
    const StringList header = getHeader(path);  // the complete header of the file
    std::string title = extract_title( find_title_inheader(header) );
    StringList tags_list = extract_tags( find_tags_inheader(header) );
    Tags tags = parse_tags(tags_list);
    bool pin = extract_pinned( find_pinned_inheader(header) );
    bool favorite = extract_favorited( find_favorite_inheader(header) );

    return new Element( path, title, tags, pin, favorite );
}

ElementsList construct_list_elements(const PathsList& f) {
    ElementsList elems;
    for (const fs::path& p : f)
        elems.push_back( construct_single_element(p) );
    return elems;
}

