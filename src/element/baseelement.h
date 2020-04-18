#ifndef BaseElement_H
#define BaseElement_H

#include <QString>
#include <vector>


class Element;
typedef std::vector<QString> StringList;
typedef std::vector<QString> PathsList;
typedef std::vector<Element*> ElementsList;
typedef std::vector<std::vector<QString> > Tags;

namespace BaseElement {

    void createNewFile(const QString& p, QString title);
    /**
     * scans all the files inside a directory
     * and returns the markdown files a vector of QString
     */
    PathsList fetch_files(const QString& dir);
    /**
     * verify if the file has a markdown extension
     */
    bool isMD(const QString& f);
    /**
     * get the header of a file, each line in a string
     * and everything in a vector of strings.
     * NOTE: a header is the lines inside two lines of "---"
     */
    StringList getHeader(const QString& path);
    /**
     * how many header items a file has
     */
    int nbItemsInHeader(const QString& fi);
    /**
     * check if a file has a header
     * "/tag/ " -> "tag"
     */
    bool hasHeader(const QString& fi);
    /**
     * get The actual values from the file or the header
     */
    QString getTitle(const StringList& header);
    bool isPinned(const StringList& header);
    bool isFavorited(const StringList& header);
    bool isDeleted(const StringList& header);
    Tags getParsedTags(const StringList& header);

    QString getTitle(const QString& p);
    Tags getTags(const QString& p);
    bool isPinned(const QString& p);
    bool isFavorited(const QString& p);
    bool isDeleted(const QString& p);
    inline Tags getParsedTags(const QString& path) { return getParsedTags(getHeader(path)); }
    /**
     * receive this:
     * "tags: [Notebooks/sheets/random, status/infinite, type/all]"
     * return the items between braquets and put them in a vector of strings
     */
    StringList getUnparsedTags(const StringList& header);
    inline StringList getUnparsedTags(const QString& path) {
        return getUnparsedTags(getHeader(path));
    }

    void setTitle(const QString& path, const QString& title);
    void setPinned(const QString& path, const bool& pinned);
    void setFavorited(const QString& path, const bool& favorite);
    void setDeleted(const QString& path, const bool& deleted);
    /**
     * look for a string from its key in the header
     */
    QString findLine(const QString& key, const StringList& header);
    inline QString findTitle(const StringList& header) { return findLine("title", header); }
    inline QString findPinned(const StringList& header) { return findLine("pinned", header); }
    inline QString findFavorited(const StringList& header) { return findLine("favorited", header); }
    inline QString findDeleted(const StringList& header) { return findLine("deleted", header); }
    inline QString findTags(const StringList& header) { return findLine("tags", header); }
    /**
     * Check if a particular key exist in the header
     */
    inline bool hasTitleKey(const StringList& header) { return !(findTitle(header).isEmpty()); }
    inline bool hasPinnedKey(const StringList& header) { return !(findPinned(header).isEmpty()); }
    inline bool hasFavoritedKey(const StringList& header) {
        return !(findFavorited(header).isEmpty());
    }
    inline bool hasDeletedKey(const StringList& header) { return !(findDeleted(header).isEmpty()); }
    inline bool hasTagsKey(const StringList& header) { return !findTags(header).isEmpty(); }
    /**
     * receive a line of a header. return the value
     * "myKey: my value" -> "my value"
     */
    QString getValue(QString line);
    /**
     * use getValue() and return the actual value with the appropriate data type
     */
    QString parseString(const QString& line);
    bool parseBool(const QString& line);
    StringList parseArray(const QString& line);


    /***********************************************************************/
    /********** MAke a header item line (key value) from a value ***********/
    /***********************************************************************/
    /**
     * prepend a header to a file
     * with a title line
     */
    void createHeader(const QString& file, const QString& title = "untitled");
    /**
     * receive: "this is a title"
     * returns: "title: this is a title"
     */
    QString makeTitleLine(QString title = "untitled");
    QString makePinnedLine(const bool& pinned = false);
    QString makeFavoritedLine(const bool& fav = false);
    QString makeDeletedLine(const bool& del = false);
    /**
     * receives: ["shallow/deep", "something/nothing"]
     * returns : "tags: [shallow/deep, something/nothing]"
     * it returns a string writable into a file to replace its previous tag
     */
    QString makeTagsLine(const StringList& lst);


    QString composeStringItem(QString key, QString value);
    QString composeBoolItem(QString key, const bool& value);
    QString composeArrayItem(QString key, const StringList& value);

    /**
     * check if the provided tags:
     *  - aren't the basic ones
     *  - don't contain forbidden characters (,)
     * NOTE: it doesn't test tag particles., but complete nested tag ("one/two/three")
     */
    bool validTagToAdd(const QString& tag);
    /**
     * receives: {"Notebooks", "summaries", "history"}
     * returns:  "Notebooks/summaries/history"
     */
    QString combineTags(const StringList& chain);
    /**
     * remove characters that gets in the way
     */
    QString& processTag(QString& tag);

    /***********************************************************************/
    /************ Insert an item into the header of a file *****************/
    /***********************************************************************/
    /***
     * receive a line ready to be appended to the header of a file
     */
    void addItemToHeader(const QString& item, const QString& path);
    void addPinnedItem(QString pinnedLine, const QString& path);
    void addFavoritedItem(QString favoritedLine, const QString& path);
    void addDeletedItem(QString deletedLine, const QString& path);
    void addTagsItem(QString tagsLine, const QString& path);
    void addTitleItem(QString titleLine, const QString& path);

    /***********************************************************************/
    /****************** Remove a specific item in the header ***************/
    /***********************************************************************/
    void removeLineFromHeader(const QString& line, const QString& path);
    void removePinnedItemFromHeader(const QString& path);
    void removeFavoritedItemFromHeader(const QString& path);
    void removeDeletedItemFromHeader(const QString& path);
    void removeTagsItemFromHeader(const QString& path);
    void removeTitleItemFromHeader(const QString& path);
    /**
     * open a file, return each line of it
     * as an entry in a vector<string>
     */
    StringList getFileContent(const QString& file);
    void writeContentToFile(const StringList& content, const QString& file);
    /**
     * remove the quotations around strings
     * call this function on every title to remove them
     */
    void remove_quotations(QString& str);
    /**
     * find a line in a text file and replace it
     */
    bool replace(const QString& old_str, const QString& new_str, const QString& path);
    /**
     * add a particle before and after the string
     */
    void enwrap(QString& str, QString before, QString after);
    void unwrap(QString& str, QString before, QString after);

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
     * an earlier version of this function was found in:
          https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    */
    StringList split(const QString& s, const QString& delimiter = "/");

}   // namespace BaseElement

#endif   // BaseElement_H
