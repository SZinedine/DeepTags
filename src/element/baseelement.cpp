#include "baseelement.h"
#include <QDebug>
#include <QDirIterator>
#include <QStringRef>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <algorithm>
#include <cctype>
#include <iostream>

void BaseElement::createNewFile(const QString& p, QString title) {
    title = title.simplified();
    if (title.isEmpty()) title = "untitled";
    const QString title_line = makeTitleLine(title);

    StringList content;
    content.push_back(QString::fromUtf8("---"));
    content.push_back(title_line);
    content.push_back(QString::fromUtf8("---"));
    content.push_back(QString::fromUtf8("\n"));
    content.push_back(title);
    content.push_back(QString::fromUtf8("=========="));

    writeContentToFile(content, QString(p));
}

PathsList BaseElement::fetch_files(const QString& dir) {
    PathsList list;

    QDir d(dir);
    if (!d.exists()) return list;

    QDirIterator p(dir, QDir::Files, QDirIterator::Subdirectories);
    while (p.hasNext()) {
        QString pp = p.next();
        if (isMD(pp)) list.push_back(pp);
    }

    return list;
}


bool BaseElement::isMD(const QString& f) {
    QString ext = QFileInfo(f).suffix();
    return (ext == "md" || ext == "markdown" || ext == "Markdown" || ext == "MD");
}

bool BaseElement::hasHeader(const QString& fi) {
    return (nbItemsInHeader(fi) > 0);
}


int BaseElement::nbItemsInHeader(const QString& fi) {
    return getHeader(fi).size();
}


StringList BaseElement::getHeader(const QString& path) {
    StringList header;
    StringList content(getFileContent(path));
    if (content.empty()) return StringList{};
    if (content.at(0) != "---") return StringList{};


    int headerMark = 0;   // how many times "---" have been encountered before stopping (2)
    for (QString& line : content) {
        if (headerMark == 2) break;   // break if we finished the header
        if (line == "---")
            headerMark++;
        else
            header.push_back(line);   // add to the vector if the current line is metadata
    }

    return header;
}


QString BaseElement::getTitle(const StringList& header) {
    QString found = findTitle(header);
    if (found.isEmpty()) return QString();
    found = getValue(found);
    remove_quotations(found);
    return found;
}

Tags BaseElement::getParsedTags(const StringList& header) {
    StringList unparsed = getUnparsedTags(header);
    if (unparsed.empty()) return Tags{};

    Tags res;
    for (const QString& i : unparsed) res.push_back(split(i));
    return res;
}

StringList BaseElement::getUnparsedTags(const StringList& header) {
    QString found = findTags(header);
    if (found.isEmpty()) return StringList();
    StringList res = parseArray(found);
    for (QString& s : res) processTag(s);
    return res;
}

bool BaseElement::isPinned(const StringList& header) {
    QString found = findPinned(header);
    if (found.isEmpty()) return false;
    return parseBool(found);
}

bool BaseElement::isFavorited(const StringList& header) {
    QString found = findFavorited(header);
    if (found.isEmpty()) return false;
    return parseBool(found);
}

bool BaseElement::isDeleted(const StringList& header) {
    QString found = findDeleted(header);
    if (found.isEmpty()) return false;
    return parseBool(found);
}


QString BaseElement::getTitle(const QString& path) {
    return getTitle(getHeader(path));
}

bool BaseElement::isPinned(const QString& path) {
    return isPinned(getHeader(path));
}

bool BaseElement::isFavorited(const QString& path) {
    return isFavorited(getHeader(path));
}

bool BaseElement::isDeleted(const QString& path) {
    return isDeleted(getHeader(path));
}


// used by all other functions that search for a key in a header
QString BaseElement::findLine(const QString& key, const StringList& header) {
    const QString k = key.simplified() + ":";
    for (QString s : header) {
        s = s.simplified();
        if (s.startsWith(k)) return s;
    }
    return QString("");
}


void BaseElement::setTitle(const QString& path, const QString& title) {
    if (!hasTitleKey(getHeader(path))) return;
    if (title.isEmpty()) return;
    QString t        = makeTitleLine(title.simplified());
    QString old_line = findTitle(getHeader(path));
    replace(old_line, t, path);
}

void BaseElement::setPinned(const QString& path, const bool& pin) {
    if (!hasPinnedKey(getHeader(path))) return;
    QString pinned = makePinnedLine(pin);
    QString old    = findPinned(getHeader(path));
    if (old.isEmpty()) return;
    replace(old, pinned, path);
}

void BaseElement::setFavorited(const QString& path, const bool& favorited) {
    if (!hasFavoritedKey(getHeader(path))) return;
    QString fav = makeFavoritedLine(favorited);
    QString old = findFavorited(getHeader(path));
    if (old.isEmpty()) return;
    replace(old, fav, path);
}

void BaseElement::setDeleted(const QString& path, const bool& deleted) {
    if (!hasDeletedKey(getHeader(path))) return;
    QString del = makeDeletedLine(deleted);
    QString old = findDeleted(getHeader(path));
    if (old.isEmpty()) return;
    replace(old, del, path);
}


QString BaseElement::getValue(QString line) {
    line    = line.simplified();
    int pos = line.indexOf(':') + 2;
    QStringRef ref(&line, pos, line.size() - pos);
    return ref.toString().simplified();
}


QString BaseElement::parseString(const QString& line) {
    return getValue(line);
}


bool BaseElement::parseBool(const QString& line) {
    QString val = getValue(line);
    remove_quotations(val);
    return (val == "true");
}

StringList BaseElement::parseArray(const QString& line) {
    QString val = getValue(line);
    unwrap(val, "[", "]");

    StringList res = split(val, ",");
    for (QString& i : res) remove_quotations(i);

    return res;
}


void BaseElement::unwrap(QString& str, QString before, QString after) {
    if (str.isEmpty()) return;
    str = str.simplified();
    if (!str.startsWith(before) && !str.endsWith(after)) return;
    QStringRef ref(&str, 1, str.size() - 2);
    str = ref.toString();
}

void BaseElement::enwrap(QString& str, QString before, QString after) {
    str = str.simplified();
    str = before + str + after;
}

void BaseElement::remove_quotations(QString& str) {
    unwrap(str, "\"", "\"");
    unwrap(str, "\'", "\'");
}


StringList BaseElement::split(const QString& s, const QString& delimiter) {
    auto qsl = s.split(delimiter);
    StringList res;
    for (QString i : qsl) res.push_back(i);
    return res;
}


void BaseElement::createHeader(const QString& file, const QString& title) {
    if (hasHeader(file)) return;
    StringList header;
    header.push_back(QString::fromUtf8("---"));
    header.push_back(makeTitleLine(title));
    header.push_back(QString::fromUtf8("---\n"));

    StringList entire_file = getFileContent(file);
    for (const QString& i : entire_file) header.push_back(i);
    writeContentToFile(header, file);
}


QString BaseElement::makeTitleLine(QString title) {
    title = (title.isEmpty()) ? "untitled" : title;
    return composeStringItem("title", title);
}


QString BaseElement::makePinnedLine(const bool& pinned) {
    return composeBoolItem("pinned", pinned);
}

QString BaseElement::makeFavoritedLine(const bool& favorited) {
    return composeBoolItem("favorited", favorited);
}

QString BaseElement::makeDeletedLine(const bool& del) {
    return composeBoolItem("deleted", del);
}

QString BaseElement::makeTagsLine(const StringList& lst) {
    return composeArrayItem("tags", lst);
}


QString BaseElement::composeStringItem(QString key, QString value) {
    key = key.simplified();
    key.append(": ");
    enwrap(value, "'", "'");
    return key + value;
}

QString BaseElement::composeBoolItem(QString key, const bool& value) {
    key       = key.simplified();
    QString v = ((value) ? "true" : "false");
    key.append(": " + v);
    return key;
}

QString BaseElement::composeArrayItem(QString key, const StringList& value) {
    key = key.simplified() + ": ";
    QString val;

    for (StringList::size_type i = 0; i < value.size(); i++) {
        val.append(value[i].simplified());
        if (i != value.size() - 1) val.append(", ");
    }
    enwrap(val, "[", "]");
    return key + val;
}

bool BaseElement::validTagToAdd(const QString& tag) {
    // test if reserved tags
    if (tag.isEmpty()) return false;
    for (const QString& s : { "All Notes", "Notebooks", "Favorite", "Untagged", "Trash" })
        if (s == tag) {
            std::cerr << "Error. Cannot add a basic tag to a file\n";
            return false;
        }

    // list of forbidden characters
    QStringList forbidden{ "," };
    for (auto s : forbidden)
        if (tag.contains(s)) return false;

    // check if it starts or ends with a slash
    if (tag.startsWith("/") || tag.endsWith("/")) return false;

    return true;
}


void BaseElement::addItemToHeader(const QString& item, const QString& path) {
    if (!hasHeader(path)) createHeader(path, QFileInfo(path).baseName());

    StringList entire_file = getFileContent(path);
    StringList newFile;

    int headerIndic = 0;
    for (const QString& line : entire_file) {
        if (line == "---") headerIndic++;
        if (headerIndic == 2) {
            newFile.push_back(item);
            headerIndic++;
        }
        newFile.push_back(line);
    }
    writeContentToFile(newFile, path);
}


void BaseElement::addPinnedItem(QString pinnedLine, const QString& path) {
    if (hasPinnedKey(getHeader(path))) return;
    pinnedLine = pinnedLine.simplified();
    if (pinnedLine == "pinned: true" || pinnedLine == "pinned: false")
        addItemToHeader(pinnedLine, path);
}

void BaseElement::addFavoritedItem(QString favoritedLine, const QString& path) {
    if (hasFavoritedKey(getHeader(path))) return;
    favoritedLine = favoritedLine.simplified();
    if (favoritedLine == "favorited: true" || favoritedLine == "favorited: false")
        addItemToHeader(favoritedLine, path);
}


void BaseElement::addDeletedItem(QString deletedLine, const QString& path) {
    if (hasDeletedKey(getHeader(path))) return;
    deletedLine = deletedLine.simplified();
    if (deletedLine == "deleted: true" || deletedLine == "deleted: false")
        addItemToHeader(deletedLine, path);
}


void BaseElement::addTagsItem(QString tagsLine, const QString& path) {
    if (hasTagsKey(getHeader(path))) return;
    tagsLine = tagsLine.simplified();
    if (tagsLine.size() < 9) return;
    addItemToHeader(tagsLine, path);
}

void BaseElement::addTitleItem(QString titleLine, const QString& path) {
    if (hasTitleKey(getHeader(path))) return;
    // if (!hasHeader(path)) createHeader(path, getValue(titleLine));
    else
        addItemToHeader(titleLine.simplified(), path);
}

void BaseElement::removeLineFromHeader(const QString& line, const QString& path) {
    if (!hasHeader(path)) return;
    if (line.isEmpty()) return;
    StringList content = getFileContent(path);
    StringList newContent;
    for (const QString& l : content) {
        if (l == line) continue;
        newContent.push_back(l);
    }
    writeContentToFile(newContent, path);
}


void BaseElement::removePinnedItemFromHeader(const QString& path) {
    if (!hasPinnedKey(getHeader(path))) return;
    QString str = findPinned(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeFavoritedItemFromHeader(const QString& path) {
    if (!hasFavoritedKey(getHeader(path))) return;
    QString str = findFavorited(getHeader(path));
    removeLineFromHeader(str, path);
}

void BaseElement::removeDeletedItemFromHeader(const QString& path) {
    if (!hasDeletedKey(getHeader(path))) return;
    QString str = findDeleted(getHeader(path));
    removeLineFromHeader(str, path);
}

void BaseElement::removeTagsItemFromHeader(const QString& path) {
    if (!hasTagsKey(getHeader(path))) return;
    QString str = findTags(getHeader(path));
    removeLineFromHeader(str, path);
}


void BaseElement::removeTitleItemFromHeader(const QString& path) {
    if (!hasTitleKey(getHeader(path))) return;
    QString str = findTitle(getHeader(path));
    removeLineFromHeader(str, path);
}

QString& BaseElement::processTag(QString& tag) {
    tag = tag.simplified();
    while (tag.startsWith("/")) tag = QStringRef(&tag, 1, tag.size() - 1).toString().simplified();
    while (tag.endsWith("/")) tag = QStringRef(&tag, 0, tag.size() - 1).toString().simplified();
    return tag;
}


QString BaseElement::combineTags(const StringList& chain) {
    QString res;
    for (StringList::size_type i = 0; i < chain.size(); i++) {
        res.append(chain[i]);
        if (i != chain.size() - 1) res.append("/");
    }
    return res;
}


bool BaseElement::replace(const QString& old_str, const QString& new_str, const QString& path) {
    if (old_str == new_str) return true;
    StringList entire_file = getFileContent(path);
    if (entire_file.empty()) return false;

    // replace the string in the vector
    bool found = false;
    for (QString& s : entire_file) {
        if (s == old_str) {
            s     = new_str;
            found = true;
            break;
        }
    }
    if (!found) return false;

    writeContentToFile(entire_file, path);
    return true;
}


StringList BaseElement::getFileContent(const QString& file) {
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while trying to get file content of: " << file;
        return StringList{};
    }

    StringList entire_file;
    QTextStream f(&qf);
    f.setCodec("UTF-8");
    while (!f.atEnd()) entire_file.push_back(f.readLine());
    return entire_file;
}

void BaseElement::writeContentToFile(const StringList& content, const QString& file) {
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error while trying to write content into: " << file;
        return;
    }
    QTextStream f(&qf);
    f.setCodec("UTF-8");
    for (auto& line : content) f << line << "\n";
    f.flush();
}
