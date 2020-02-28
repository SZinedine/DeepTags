#include <QToolBar>
#include <filesystem>
#include "./../../3rdParty/qmarkdowntextedit/qmarkdowntextedit.h"

namespace fs = std::filesystem;

class EditorWidget : public QWidget {
    Q_OBJECT
public:
    EditorWidget(QWidget* parent = nullptr);

public slots:
    void open(const QString& path);
    void save();
    void closeFile();
    inline void reload() { open(m_currentPath); }

signals:
    void openedFile(QString file);
    void savedFile(QString file);
    void closedFile(QString file);

private:
    QMarkdownTextEdit* m_editor;
    // QToolBar* m_toolBar;
    QString m_currentPath;
    QString m_fileContent;
};