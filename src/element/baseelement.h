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
typedef std::vector<std::vector<std::string>> Tags;

namespace BaseElement {

    void createNewFile(const fs::path& p, std::string title);
    
    /**
     * scans all the files of a directory and return the markdown files inside a list of paths
     */
    PathsList fetch_files(const std::string& dir);
    /**
     * get the header of a file, each line in a string
     * and everything in a vector of strings.
     * NOTE: a header is the lines inside two lines of "---"
     */
    StringList getHeader(const fs::path& fi);
    /**
     * verify if the file has a markdown extension
     */
    bool isMD(const fs::path &f);

    /**
     * how many header items a file has
     */
    int nbItemsInHeader(const fs::path& fi);


    /********************************************************/
    /**********  Extract the value from a header line *******/
    /********************************************************/
    /**
     * receives a line of the header of a file, which are
     * written in a key-value fashion.
     * returns the value
     ********************************************************/

    /** Extract the title from a string in this form: "title: this is the title"
     */
    std::string extract_title(const std::string& tit);
    /**
     * receives : "pinned: true"
     * returns  : bool(true)
     */
    bool extract_pinned(const std::string& pi);
    bool extract_favorited(const std::string& fav);
    bool extract_deleted(const std::string& fav);
    /**
     * receive this:
     * "tags: [Notebooks/sheets/random, status/infinite, type/all]"
     * return the items between braquets and put them in a vector of strings
     */
    StringList extract_tags(const std::string& s);
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
    StringList split_single_tag(const std::string& s, const std::string& delimiter="/");
    /**
     * transform the raw tags retrieved from the file,
     * parse them using split_single_tag()
     * and put them inside a vector of vectors of strings (Tags typedef)
     */
    Tags parse_tags(const StringList& raw_tags);
    


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
    /****************** Change a header item in a file **********************/
    /***********************************************************************/
    /***
     * the header item already exists in the header.
     * these functions are ment to change the value of the key
     */
    void changeTitleInFile(std::string title, const fs::path& path);
    void changePinnedInFile(const bool& pinned, const fs::path& path);
    void changeFavoritedInFile(const bool& favorited, const fs::path& path);
    void changeDeletedInFile(const bool& deleted, const fs::path& path);

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
    

/********************  protected ***********************/
    /**
     * find a line in a text file and replace it
     */
    bool replace(const std::string& old_str, const std::string& new_str, const fs::path& path);
    
    StringList getFileContent(const fs::path file);
    void writeContentToFile(const StringList& content, const fs::path file);
    inline bool toBool(const std::string& s)     {    return ((s=="true")?true:false);  }
    inline std::string toStr(const bool& b)      {    return ((b)?"true":"false");      }

    void ltrim(std::string &s);
    void rtrim(std::string &s);
    /**
     * Some lines are surrounded with quotations,
     * call this function on every title to remove them
     */
    void remove_quotations(std::string& str);

}

#endif // BaseElement_H
