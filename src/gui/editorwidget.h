#include <QFileSystemWatcher>
#include <QToolBar>
#include "./../../3rdParty/qmarkdowntextedit/qmarkdowntextedit.h"

#ifdef USE_BOOST
    #include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
    #include <filesystem>
namespace fs = std::filesystem;
#endif

class EditorWidget : public QWidget {
    Q_OBJECT
public:
    EditorWidget(QWidget* parent = nullptr);

    void open(QString path);
    void display(QString path);
    void closeFile();
    void reload();

signals:
    void openedFile(QString file);
    void savedFile(QString file);
    void closedFile(QString file);

private:
    QMarkdownTextEdit* m_editor;
    QFileSystemWatcher* m_watcher;
    // QToolBar* m_toolBar;
    QString m_currentPath;
    QString m_fileContent;
};
