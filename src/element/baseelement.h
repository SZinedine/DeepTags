#ifndef BaseElement_H
#define BaseElement_H

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class Element;
typedef std::vector<std::string> StringList;
typedef std::vector<fs::path> PathsList;
typedef std::vector<Element*> ElementsList;
typedef std::vector< std::vector<std::string> > Tags;

namespace BaseElement {

    void createNewFile(const fs::path& p, std::string title);
    
    /**
     * scans all the files inside a directory 
     * and returns the markdown files a vector of fs::path
     */
    PathsList fetch_files(const std::string& dir);
    /**
     * get the header of a file, each line in a string
     * and everything in a vector of strings.
     * NOTE: a header is the lines inside two lines of "---"
     */
    StringList getHeader(const fs::path& path);
    /**
     * verify if the file has a markdown extension
     */
    bool isMD(const fs::path &f);

    /**
     * how many header items a file has
     */
    int nbItemsInHeader(const fs::path& fi);

    

    /*********** New functions ***********/


    // get The actual values from the file or the header
    std::string getTitle(const fs::path& p);
    Tags getTags(const fs::path& p);
    bool isPinned(const fs::path& p);
    bool isFavorited(const fs::path& p);
    bool isDeleted(const fs::path& p);

    std::string getTitle(const StringList& header);
    bool isPinned(const StringList& header);
    bool isFavorited(const StringList& header);
    bool isDeleted(const StringList& header);
    Tags getParsedTags(const StringList& header);
    inline auto getParsedTags(const fs::path& path) { return getParsedTags(getHeader(path)); }
    /**
     * receive this:
     * "tags: [Notebooks/sheets/random, status/infinite, type/all]"
     * return the items between braquets and put them in a vector of strings
     */
    StringList getUnparsedTags(const StringList& header);
    inline auto getUnparsedTags(const fs::path& path) { return getUnparsedTags(getHeader(path)); }

    void setTitle(const fs::path& path, const std::string& title);
    void setPinned(const fs::path& path, const bool& pinned);
    void setFavorited(const fs::path& path, const bool& favorite);
    void setDeleted(const fs::path& path, const bool& deleted);

    /**
     * look for a string from its key in the header
     */
    std::string findLine(const std::string& key, const StringList& header);
    inline std::string findTitle(const StringList& header) { return findLine("title", header);}
    inline std::string findPinned(const StringList& header) { return findLine("pinned", header);}
    inline std::string findFavorited(const StringList& header){ return findLine("favorited", header);}
    inline std::string findDeleted(const StringList& header) { return findLine("deleted", header);}
    inline std::string findTags(const StringList& header) { return findLine("tags", header);}

    
    inline bool hasTitleKey(const StringList& header) { return !(findTitle(header).empty()); }
    inline bool hasPinnedKey(const StringList& header) { return !(findPinned(header).empty()); }
    inline bool hasFavoritedKey(const StringList& header) { return !(findFavorited(header).empty()); }
    inline bool hasDeletedKey(const StringList& header) { return !(findDeleted(header).empty()); }
    inline bool hasTagsKey(const StringList& header) { return !(findTags(header).empty()); }

    /**
     * receive a line of a header. return the value
     * "myKey: my value" -> "my value"
     */
    std::string getValue(std::string line);
    /**
     * use getValue() and return the actual value with the appropriate data type
     */
    std::string parseString(const std::string& line);
    bool parseBool(const std::string& line);
    StringList parseArray(const std::string& line);

    /**
     * add a particle before and after the string
     */
    void enwrap(std::string& str, const std::string& before, const std::string& after);
    void unwrap(std::string& str, const std::string& before, const std::string& after);




    /*********************************************************************/
    /*********************************************************************/
    /*********************************************************************/
    /*********************************************************************/
    /*********************************************************************/
    /*********************************************************************/
    /*********************************************************************/




    /**
     * split a single string into particles and return them in a vector of strings
     * for example:
     *      input: "Notebooks/sheets/random"
     *      output: {"Notebooks", "sheets, "random"}
     *
     * use in this way: split("Notebooks/sheets/random", "/");
     *
     * since I have dropped the usage of yaml-cpp, this function
     * is also used to separate the tags block by passing ", " as a delimiter.
     *
     * function found in:
          https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    */
    StringList split(const std::string& s, const std::string& delimiter="/");
    


    /********************************************************/
    /************  find something in header   ***************/
    /********************************************************/
    /**
     * functions that receive the list of header lines
     * each of them is designed to find a particular line
     * for example, for title. the function looks for a line
     * that begins with "title: "
     *
     * these methods return the raw line. other methods (extract_x)
     * have the ability to extract the value
     ********************************************************/
    std::string find_title_inheader(const StringList& header);
    std::string find_tags_inheader(const StringList& header);
    std::string find_pinned_inheader(const StringList& header);
    std::string find_favorite_inheader(const StringList& header);
    std::string find_deleted_inheader(const StringList& header);

    

    /***********************************************************************/
    /********** MAke a header item line (key value) from a value ***********/
    /***********************************************************************/
    /**
     * prepend a header to a file
     * with a title line
     */
    void createHeader(const fs::path& file, const std::string& title="untitled");
    /**
     * receive: "this is a title"
     * returns: "title: this is a title"
     */
    std::string makeTitleLine(std::string title="untitled");
    std::string makePinnedLine(const bool& pinned=false);
    std::string makeFavoritedLine(const bool& fav=false);
    std::string makeDeletedLine(const bool& fav=false);
    /**
     * receives: ["shallow/deep", "something/nothing"]
     * returns : "tags: [shallow/deep, something/nothing]"
     * it returns a string writable into a file to replace its previous tag
     * old function name: combineTagsIntoString
     */
    std::string makeTagsLine(const StringList& lst);
    
    
    
    /***********************************************************************/
    /************************** Check Methods ******************************/
    /***********************************************************************/
    /**
     * Check if a the file has a specific item. ex: ("tags: [...]")
     */
    bool hasTagItem(const fs::path& f);
    bool hasTitleItem(const fs::path& f);
    bool hasPinnedItem(const fs::path& f);
    bool hasFavoritedItem(const fs::path& f);
    bool hasDeletedItem(const fs::path& f);
    /**
     * check if the provided tags:
     *     - aren't the basic onces
     *  - don't contain forbidden characters (,)
     * NOTE: it doesn't test tag particles., but complete nested tag ("one/two/three")
     */
    bool validTagToAdd(const std::string& tag);
    /**
     * check if a file has a Notable like header
     */
    bool hasHeader(const fs::path& fi);
    
    /***********************************************************************/
    /************ Insert an item into the header of a file *****************/
    /***********************************************************************/
    /***
     * receive a line ready to be appended to the header of a file
     */
    void addItemToHeader(const std::string& item, const fs::path& path);
    void addPinnedItem(std::string pinnedLine, const fs::path& path);
    void addFavoritedItem(std::string favoritedLine, const fs::path& path);
    void addDeletedItem(std::string deletedLine, const fs::path& path);
    void addTagsItem(std::string tagsLine, const fs::path& path);

    /***********************************************************************/
    /****************** Remove a specific item in the header ***************/
    /***********************************************************************/
    void removeLineFromHeader(const std::string& line, const fs::path path);
    void removePinnedItemFromHeader(const fs::path& path);
    void removeFavoritedItemFromHeader(const fs::path& path);
    void removeDeletedItemFromHeader(const fs::path& path);
    void removeTagsItemFromHeader(const fs::path& path);
    void removeTitleItemFromHeader(const fs::path& path);

    /**
     * receives: {"Notebooks", "summaries", "history"}
     * returns:  "Notebooks/summaries/history"
     */
    std::string combineTags(const StringList& chain);

    /**
     * remove trailing spaces
     * found in: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
     */
    void trim(std::string &s);
    
    /**
     * find a line in a text file and replace it
     */
    bool replace(const std::string& old_str, const std::string& new_str, const fs::path& path);
    /**
     * open a file, return each line of it
     * as an entry in a vector<string>
     */
    StringList getFileContent(const fs::path file);
    void writeContentToFile(const StringList& content, const fs::path file);
    inline bool toBool(const std::string& s) { return ((s=="true")?true:false); }
    inline std::string toStr(const bool& b)  { return ((b)?"true":"false");     }

    /**
     * Some lines are surrounded with quotations,
     * call this function on every title to remove them
     */
    void remove_quotations(std::string& str);








    /***********************************************************************/
    /********************* deprecated **************************/
    /***********************************************************************/






}

#endif // BaseElement_H
