#include "Settings.h"
#include <QHash>
#include <QSettings>
#include <QSize>
#include <QVector>

const auto mainGroup                     = QStringLiteral("main");
const auto pinnedTagsLabel               = QStringLiteral("pinned_tags");
const auto coloredTagsLabel              = QStringLiteral("colored_tags");
const auto documentViewerVisibilityLabel = QStringLiteral("document_viewer_visibility");
const auto splitterSizeLabel             = QStringLiteral("splitter_size");
const auto windowSizeLabel               = QStringLiteral("window_size");
const auto dataDirectoryLabel            = QStringLiteral("data_directory");
const auto externalReadersGroup          = QStringLiteral("markdown_editors");
const auto externalReadersLabel          = QStringLiteral("list");
const auto recentFilesGroup              = QStringLiteral("files");
const auto recentFilesListLabel          = QStringLiteral("recently_opened_files");
const auto themeLabel                    = QStringLiteral("theme");
constexpr int maxSizeRecentFilesList     = 15;

bool contains(const QString& group, const QString& label);
QVariant getValue(const QString& group, const QString& label);
void saveValue(const QString& group, const QString& label, const QVariant& value);

namespace Ui::Settings {


void saveUiSettings(const QSize& windowSize, const QByteArray& splitterState) {
    saveValue(mainGroup, windowSizeLabel, QVariant(windowSize));   // size of the window
    saveValue(mainGroup, splitterSizeLabel, QVariant(splitterState));
}


QByteArray loadSplitterState() {
    return getValue(mainGroup, splitterSizeLabel).toByteArray();
}


void saveSplitterState(QByteArray& data) {
    saveValue(mainGroup, splitterSizeLabel, data);
}


QSize loadWindowSize() {
    return getValue(mainGroup, windowSizeLabel).toSize();
}


QVector<QString> getPinnedTags() {
    return getValue(mainGroup, pinnedTagsLabel).toStringList().toVector();
}


void setPinnedTags(const QVector<QString>& tags) {
    saveValue(mainGroup, pinnedTagsLabel, QVariant(tags.toList()));
}


QHash<QString, QVariant> getColoredTags() {
    return getValue(mainGroup, coloredTagsLabel).toHash();
}


void setColoredTags(const QHash<QString, QVariant>& colors) {
    saveValue(mainGroup, coloredTagsLabel, QVariant(colors));
}


void saveDocumentViewerVisibility(bool use) {
    saveValue(mainGroup, documentViewerVisibilityLabel, QVariant(use));
}


bool loadDocumentViewerVisibility() {
    return getValue(mainGroup, documentViewerVisibilityLabel).toBool();
}


void saveDataDirectory(const QString& value) {
    saveValue(mainGroup, dataDirectoryLabel, value);
}


QString loadDataDirectory() {
    return getValue(mainGroup, dataDirectoryLabel).toString();
}


void saveExternalReaders(const QStringList& value) {
    saveValue(externalReadersGroup, externalReadersLabel, value);
}


QStringList loadExternalReaders() {
    return getValue(externalReadersGroup, externalReadersLabel).toStringList();
}


QString loadDefaultExternalReaders() {
    if (auto lst = loadExternalReaders(); !lst.isEmpty()) {
        return lst.at(0);
    }

    return "";
}


void saveRecentFiles(const QStringList& value) {
    if (maxSizeRecentFilesList < value.size()) {
        saveValue(recentFilesGroup, recentFilesListLabel,
                  QStringList(value.begin(), value.begin() + maxSizeRecentFilesList));

    } else {
        saveValue(recentFilesGroup, recentFilesListLabel, value);
    }
}


void addRecentFile(const QString& value) {
    auto lst = loadRecentFiles();
    if (lst.contains(value)) {
        lst.removeAll(value);
        return;
    }

    lst.push_front(value);
    saveRecentFiles(lst);
}


QStringList loadRecentFiles() {
    return getValue(recentFilesGroup, recentFilesListLabel).toStringList();
}


void saveTheme(const QString& value) {
    saveValue(mainGroup, themeLabel, value);
}


QString loadTheme() {
    return getValue(mainGroup, themeLabel).toString();
}

}   // namespace Ui::Settings


void saveValue(const QString& group, const QString& label, const QVariant& value) {
    QSettings s;
    s.beginGroup(group);
    s.setValue(label, value);
    s.endGroup();
}


QVariant getValue(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    QVariant val = s.value(label);
    s.endGroup();
    return val;
}


bool contains(const QString& group, const QString& label) {
    QSettings s;
    s.beginGroup(group);
    const bool val = s.contains(label);
    s.endGroup();
    return val;
}
