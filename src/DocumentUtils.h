#ifndef DOCUMENTUTILS__H
#define DOCUMENTUTILS__H

#include <QString>
#include <optional>

using QStringVector = QVector<QString>;
namespace Doc {
class Document;
}

/* header keys */
const auto pinnedKey    = QStringLiteral("pinned");
const auto deletedKey   = QStringLiteral("deleted");
const auto favoritedKey = QStringLiteral("favorited");
const auto titleKey     = QStringLiteral("title");
const auto tagsKey      = QStringLiteral("tags");
const auto trueStr      = QStringLiteral("true");
const auto falseStr     = QStringLiteral("false");

namespace Doc::Utils {

/**
 * extract a header from the file content
 */
[[nodiscard]] QStringVector getHeader(const QStringVector& fileContent);
/**
 * return a header from a file
 */
[[nodiscard]] QStringVector getHeader(const QString& path);
/**
 * return the content of an entire file, each line is a string
 */
[[nodiscard]] QStringVector getFileContent(const QString& file);
/**
 * return the content of an entire file as a single string
 */
[[nodiscard]] QString getFileContentAsString(const QString& file);
/**
 * look in the header for an entry that starts with a key and return the entire line
 */
[[nodiscard]] QString getHeaderLine(const QString& key, const QStringVector& header);
/**
 * look in the header for an entry and return it as a string
 */
[[nodiscard]] QString getHeaderValue(const QString& key, const QStringVector& header);
/**
 * add a line to a header, or change its value if it already exists
 */
bool setHeaderValue(const QString& key, const QString& value, QStringVector& header);
/**
 * check if a header contains a key
 */
bool hasHeaderKey(const QString& key, const QStringVector& header);
/**
 * delete a line that has a key in a header
 */
bool deleteHeaderLine(const QString& key, QStringVector& header);
/**
 * check if a file contains a header. If not, create one with a title
 */
void createHeader(const QString& file, QString title = "");
/**
 * replace the header inside the file by the one provided as argument
 */
void replaceHeader(Document* document, const QStringVector& newHeader);
/**
 * Write a sequence of lines into a file
 */
bool writeContentToFile(const QStringVector& content, const QString& filepath);
/**
 * find the tag line inside the header, and return the content in vector
 */
[[nodiscard]] QStringVector splitTags(const QStringVector& header);
/**
 * "a/b/c" => {a, b, c}
 */
[[nodiscard]] QStringVector deconstructTag(const QString& tag);
/**
 * in the provided directory, look for any valid file and return them in a vector
 */
[[nodiscard]] QStringVector getPathList(const QString& directory);
/**
 * check if a file as a markdown extension
 */
[[nodiscard]] bool isMD(const QString& file);
/**
 *  produce a header entry line
 */
[[nodiscard]] QString composeHeaderLine(const QString& key, const QString& value);
/**
 * turn a list of strings into its string representation
 */
[[nodiscard]] QString arrayToString(const QStringVector& array);
/**
 * modify a header entry and write the result into a file.
 * return true if something is changed. false if not.
 */
bool setDocumentTitle(Doc::Document* document, const QString& value);
/**
 * rename a document
 */
void setDocumentPath(Doc::Document* document, const QString& documentTitle);
/**
 * applied the pinned value to the Document and change the underlying file accordingly
 */
bool setDocumentPinned(Doc::Document* document, bool value);
/**
 * applied the favorited value to the Document and change the underlying file accordingly
 */
bool setDocumentFavorited(Doc::Document* document, bool value);
/**
 * applied the deleted value to the Document and change the underlying file accordingly
 */
bool setDocumentDeleted(Doc::Document* document, bool value);
/**
 * permanently delete from disk the underlying file of a Document
 */
bool permanentlyDeleteFile(const QString& path);
/**
 * add a tag value to the Document and change the underlying file accordingly
 */
bool addDocumentTag(Doc::Document* document, QString value);
/**
 * delete a single tag string.
 * If the tag list becomes empty,
 * the whole tag entry is removed from the header.
 */
bool delDocumentTag(Doc::Document* document, const QString& value);
/**
 * remove whitespaces and the obvious invalid chars
 */
QString& sanitizeTag(QString& tag);
/**
 * sanitize the tag and check if it is valid
 * special tags being invalid tags because this function is meant to add custom tags
 */
bool isValidTag(const QString& tag);
/**
 * split a single string into particles and return them in a vector of strings
 * for example:
 *      input: "Notebooks/sheets/random"
 *      output: {"Notebooks", "sheets, "random"}
 */
QStringVector split(const QString& s, const QString& delimiter);
/**
 * create a new file from the path and the title
 */
std::optional<QString> createNewFile(const QString& path, QString title = "");

}   // namespace Doc::Utils

#endif   // DOCUMENTUTILS__H
