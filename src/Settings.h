#ifndef SETTINGS__H
#define SETTINGS__H

#include <QString>

class QVariant;
class QSize;

namespace Ui::Settings {

QVector<QString> getPinnedTags();
QHash<QString, QVariant> getColoredTags();
void setColoredTags(const QHash<QString, QVariant>& colors);
void setPinnedTags(const QVector<QString>& tags);
void setPinnedTag(const QString& tag);

void saveUiSettings(const QSize& windowSize, const QByteArray& splitterState);
QSize loadWindowSize();
QByteArray loadSplitterState();
void saveSplitterState();

/**
 * display the document viewer or not
 */
void saveDocumentViewerVisibility(bool use);
bool loadDocumentViewerVisibility();
bool hasLoadDocumentViewerVisibility();

/**
 * Save/Load the Data Directory
 */
void saveDataDirectory(const QString& value);
QString loadDataDirectory();

/**
 * Save/Load external readers
 */
void saveExternalReaders(const QStringList& value);
QStringList loadExternalReaders();
QString loadDefaultExternalReaders();

/**
 * Save/Load recent files
 */
void saveRecentFiles(const QStringList& value);
QStringList loadRecentFiles();
void addRecentFile(const QString& value);

/**
 * Save/Load theme
 */
void saveTheme(const QString& value);
QString loadTheme();
}   // namespace Ui::Settings

#endif   // SETTINGS__H
