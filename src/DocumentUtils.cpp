#include "DocumentUtils.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <cctype>
#include "Document.h"
#include "TagTreeItem.h"

const auto headerDelimiter = QStringLiteral("---");
const auto singleQuote     = QStringLiteral("'");

QString& enwrap(QString& str, const QString& before, const QString& after);
QString& unwrap(QString& str, const QString& before, const QString& after);

/**
 * replace all the unsupported characters for a filename
 */
QString& sanitizeFilename(QString& filename);


namespace Doc::Utils {

QStringVector getFileContent(const QString& file) {
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error while trying to get file content of: " << file;
        return {};
    }

    QStringVector entire_file;
    QTextStream stream(&qf);
    stream.setCodec("UTF-8");
    while (!stream.atEnd()) {
        entire_file.push_back(stream.readLine());
    }

    return entire_file;
}


QString getFileContentAsString(const QString& file) {
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error while trying to get file content of: " << file;
        return {};
    }

    QTextStream stream(&qf);
    stream.setCodec("UTF-8");
    return stream.readAll();
}


QStringVector getHeader(const QString& path) {
    return getHeader(getFileContent(path));
}


QStringVector getHeader(const QStringVector& fileContent) {
    QStringVector header;
    if (fileContent.empty() || fileContent.at(0) != headerDelimiter) {
        return QStringVector{};
    }

    int headerMark = 0;   // how many times "---" have been encountered before stopping (2)
    for (const QString& line : fileContent) {
        if (headerMark == 2) {
            break;   // break if we finished the header
        }

        if (line == headerDelimiter) {
            headerMark++;
        } else {
            header.push_back(line);   // add to the vector if the current line is metadata
        }
    }

    return (headerMark == 2) ? header : QStringVector{};
}


QString getHeaderLine(const QString& key, const QStringVector& header) {
    for (const QString& line : header) {
        if (line.startsWith(key)) {
            return line;
        }
    }

    return {};
}


QString getHeaderValue(const QString& key, const QStringVector& header) {
    QString line = getHeaderLine(key, header);

    if (line.isEmpty()) {
        return {};
    }

    const QStringVector splited = split(line, QStringLiteral(":"));

    if (2 < splited.size()) {
        QString res;

        for (int i = 1; i < splited.size(); i++) {
            res += splited.at(i) + QStringLiteral(":");
        }

        res.chop(1);
        return unwrap(res, singleQuote, singleQuote);
    } else if (splited.size() == 2) {
        QString res = splited.at(1);
        return unwrap(res, singleQuote, singleQuote);
    }

    return {};
}


bool setHeaderValue(const QString& key, const QString& value, QStringVector& header) {
    auto newLine = Doc::Utils::composeHeaderLine(key, value);

    if (hasHeaderKey(key, header)) {
        for (QString& line : header) {
            if (line.simplified().startsWith(key)) {
                if (line == newLine) {   // no change in value;
                    return false;
                }

                line = newLine;
                return true;
            }
        }
    }

    header.push_back(newLine);
    return true;
}


bool hasHeaderKey(const QString& key, const QStringVector& header) {
    return std::any_of(header.cbegin(), header.cend(),
                       [&key](const auto& line) { return line.simplified().startsWith(key); });
}


bool deleteHeaderLine(const QString& key, QStringVector& header) {
    for (int i = 0; i < header.size(); i++) {
        const QString& line = header.at(i);
        if (line.simplified().startsWith(key)) {
            header.removeAt(i);
            return true;
        }
    }

    return false;
}

void createHeader(const QString& file, QString title) {
    title = (title.isEmpty()) ? QFileInfo(file).baseName() : title;

    QStringVector header;
    header.push_back(headerDelimiter);
    header.push_back(composeHeaderLine(titleKey, title));
    header.push_back(headerDelimiter);
    header.push_back((""));

    QStringVector entire_file = getFileContent(file);
    for (const QString& i : entire_file) {
        header.push_back(i);
    }

    writeContentToFile(header, file);
}


void replaceHeader(Document* document, const QStringVector& newHeader) {
    if (newHeader.empty()) {
        qCritical() << "Cannot replace a header by an empty one";
        return;
    }

    if (getHeader(document->getPath()).isEmpty()) {
        createHeader(document->getPath());
    }

    auto fileContent = getFileContent(document->getPath());

    QStringVector newContent = newHeader;
    newContent.prepend(headerDelimiter);
    newContent.append(headerDelimiter);


    int delimiterCount = 0;
    for (const QString& line : fileContent) {
        if (2 <= delimiterCount) {
            newContent.push_back(line);
            continue;
        }

        if (line == headerDelimiter) {
            delimiterCount++;
        }
    }

    writeContentToFile(newContent, document->getPath());
    document->setHeader(newHeader);
    // document->load();
}


bool writeContentToFile(const QStringVector& content, const QString& file) {
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Error while trying to write content into: " << file;
        return false;
    }

    QTextStream stream(&qf);
    stream.setCodec("UTF-8");

    for (auto& line : content) {
        stream << line;
        Qt::endl(stream);
    }

    stream.flush();
    qf.close();

    return true;
}


QStringVector splitTags(const QStringVector& header) {
    QString tagLine{ getHeaderValue(tagsKey, header) };

    if (tagLine.isEmpty()) {
        return {};
    }

    unwrap(tagLine, QStringLiteral("["), QStringLiteral("]"));

    QStringVector result;
    for (const QString& tag : split(tagLine, QStringLiteral(", "))) {
        result.push_back(tag.simplified());
    }

    return result;
}


QStringVector deconstructTag(const QString& tag) {
    return split(tag, QStringLiteral("/"));
}


QStringVector getPathList(const QString& directory) {
    if (directory.isEmpty()) {
        return {};
    }

    QVector<QString> list;

    if (!QDir(directory).exists()) {
        return list;
    }

    QDirIterator p(directory, QDir::Files, QDirIterator::Subdirectories);
    while (p.hasNext()) {
        QString pp = p.next();
        if (isMD(pp)) {
            list.push_back(pp);
        }
    }

    return list;
}


bool isMD(const QString& file) {
    const auto file_ = file.toLower();
    static const std::vector<QString> extension{ ".md",    ".markdown", ".mdwn", ".mdown",
                                                 ".mdtxt", ".mdtext",   ".mkd" };
    return std::any_of(extension.begin(), extension.end(),
                       [&file_](auto ext) { return file_.endsWith(ext); });
}


QString composeHeaderLine(const QString& key, const QString& value) {
    return QString("%1: %2").arg(key, value);
}


QString arrayToString(const QStringVector& array) {
    QString res;
    auto arraySize = array.size();
    for (decltype(arraySize) i = 0; i < arraySize; i++) {
        res.append(array[i].simplified());
        if (i != arraySize - 1) {
            res.append(", ");
        }
    }

    return enwrap(res, QStringLiteral("["), QStringLiteral("]"));
}


bool setDocumentTitle(Doc::Document* document, const QString& value) {
    if (document == nullptr || document->getTitle() == value.simplified()) {
        return false;
    }

    auto header = document->getHeader();

    if (setHeaderValue(titleKey, value.simplified(), header)) {
        replaceHeader(document, header);
        document->load();
        setDocumentPath(document, value.simplified());
        return true;
    }

    return false;
}


void setDocumentPath(Doc::Document* document, const QString& documentTitle) {
    if (document == nullptr || documentTitle.isEmpty()) {
        return;
    }

    const QString oldFilePath  = document->getPath();
    const QString absolutePath = QFileInfo(oldFilePath).absolutePath();
    QString newFilePath;
    int n = 0;

    do {
        newFilePath = documentTitle;
        if (n != 0) {
            newFilePath = QString("%1 (%2)").arg(newFilePath, QString::number(n));
        }

        sanitizeFilename(newFilePath);
        newFilePath = absolutePath + "/" + newFilePath;
        if (!newFilePath.endsWith(".md")) {
            newFilePath += ".md";
        }
        n++;
    } while (QFile::exists(newFilePath) && oldFilePath != newFilePath);

    if (oldFilePath == newFilePath) {
        return;
    }

    if (QFile::rename(oldFilePath, newFilePath)) {
        document->setPath(newFilePath);
    }
}


bool setDocumentBoolValue(Doc::Document* document, bool value, const QString& key) {
    if (document == nullptr) {
        return false;
    }

    QString valueStr = (value) ? trueStr : falseStr;
    auto header      = document->getHeader();

    if (setHeaderValue(key, valueStr, header)) {
        replaceHeader(document, header);

        if (!value) {
            deleteHeaderLine(key, header);
            replaceHeader(document, header);
        }

        return true;
    }

    return false;
}


bool setDocumentPinned(Doc::Document* document, bool value) {
    if (setDocumentBoolValue(document, value, pinnedKey)) {
        document->setPinned(value);
        return true;
    }

    return false;
}


bool setDocumentFavorited(Doc::Document* document, bool value) {
    if (setDocumentBoolValue(document, value, favoritedKey)) {
        document->setFavorited(value);
        return true;
    }

    return false;
}


bool setDocumentDeleted(Doc::Document* document, bool value) {
    if (setDocumentBoolValue(document, value, deletedKey)) {
        document->setDeleted(value);
        return true;
    }

    return false;
}


bool permanentlyDeleteFile(const QString& path) {
    if (!QFile::exists(path)) {
        qCritical() << path << "does not exist.";
        return false;
    }

    if (!QFile::remove(path)) {
        qCritical() << "Error while removing " << path;
        return false;
    }

    return true;
}


bool addDocumentTag(Doc::Document* document, QString value) {
    sanitizeTag(value);
    if (document == nullptr || !isValidTag(value)) {
        return false;
    }

    if (document->getTags().contains(value.simplified())) {
        return false;
    }

    document->addTag(value);
    auto tagsStr = arrayToString(document->getTags());
    auto header  = document->getHeader();

    if (setHeaderValue(tagsKey, tagsStr, header)) {
        replaceHeader(document, header);
        return true;
    }

    return false;
}


bool delDocumentTag(Doc::Document* document, const QString& value) {
    if (document == nullptr || document->getTags().isEmpty() ||
        !document->containsExactTag(value.simplified())) {
        return false;
    }

    auto header = document->getHeader();
    auto tags   = document->getTags();

    tags.removeAll(value.simplified());

    if (tags.isEmpty()) {
        deleteHeaderLine(tagsKey, header);
    } else {
        auto tagsStr = arrayToString(tags);
        setHeaderValue(tagsKey, tagsStr, header);
    }

    replaceHeader(document, header);
    document->delTag(value);

    return true;
}


QStringVector split(const QString& s, const QString& delimiter) {
    const QStringList qsl = s.split(delimiter);
    QStringVector res;
    for (const QString& i : qsl) {
        res.push_back(i);
    }

    return res;
}


bool isValidTag(const QString& tag) {
    if (tag.isEmpty()) {
        return false;
    }

    if (Ui::Models::TagTreeItem::specialTags.contains(tag)) {
        qCritical() << "Error. Cannot add a basic tag to a file";
        return false;
    }

    QStringList forbiddenChars{ "," };
    for (const auto& s : forbiddenChars) {
        if (tag.contains(s)) {
            return false;
        }
    }

    // check if it starts or ends with a slash
    return !(tag.startsWith(QLatin1String("/")) || tag.endsWith(QLatin1String("/")));
}


QString& sanitizeTag(QString& tag) {
    tag = tag.simplified();

    while (tag.startsWith(QLatin1String("/"))) {
        tag.remove(0, 1);
    }

    while (tag.endsWith(QLatin1String("/"))) {
        tag.chop(1);
    }

    while (tag.contains(QLatin1String("//"))) {
        tag.replace(QLatin1String("//"), QLatin1String("/"));
    }

    while (tag.contains(QLatin1String("\\"))) {
        tag.replace(QLatin1String("\\"), QLatin1String("/"));
    }

    return tag;
}


std::optional<QString> createNewFile(const QString& path, QString title) {
    if (path.isEmpty()) {
        return {};
    }

    if (!QFileInfo::exists(path)) {
        return {};
    }

    title = (title.isEmpty()) ? QStringLiteral("untitled") : title;
    QString rawTitle(title);
    sanitizeFilename(title);
    QString filePath = "";
    int num          = 0;
    do {
        if (num == 0) {
            filePath = QString("%1/%2.md").arg(path, title);
        } else {
            filePath = QString("%1/%2 (%3).md").arg(path, title).arg(num);
        }

        num++;
    } while (QFile::exists(filePath));

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "failed to create: " << filePath;
        return {};
    }

    file.close();
    createHeader(filePath, title);

    // write title to the file
    auto content = getFileContent(filePath);
    content.push_back(QLatin1String(""));
    content.push_back(rawTitle);
    content.push_back(QLatin1String("=========="));
    content.push_back(QLatin1String(""));

    writeContentToFile(content, filePath);

    return filePath;
}

}   // namespace Doc::Utils


/**
 * add a particle before and after the string
 */
QString& enwrap(QString& str, const QString& before, const QString& after) {
    str = str.simplified();
    str.prepend(before);
    str.append(after);
    return str;
}

/**
 * remove a particle before and after the string
 */
QString& unwrap(QString& str, const QString& before, const QString& after) {
    str = str.simplified();

    if (str.size() < 2 || !(str.startsWith(before) && str.endsWith(after))) {
        return str;
    }

    str.chop(1);
    str.remove(0, 1);

    return str;
}


QString& sanitizeFilename(QString& filename) {
    filename = filename.simplified();

    filename.replace(QLatin1String(":"), QLatin1String(";"));
    filename.replace(QLatin1String("/"), QLatin1String("-"));
    filename.replace(QLatin1String("\\"), QLatin1String("-"));
    filename.replace(QLatin1String("|"), QLatin1String("-"));
    filename.replace(QLatin1String("<"), QLatin1String("("));
    filename.replace(QLatin1String(">"), QLatin1String(")"));
    filename.replace(QLatin1String("*"), QLatin1String("-"));
    filename.replace(QLatin1String("?"), QLatin1String(""));
    filename.replace(QLatin1String("\""), QLatin1String("'"));

    return filename;
}
