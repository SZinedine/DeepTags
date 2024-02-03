#ifndef DOCUMENTDISPLAYER__H
#define DOCUMENTDISPLAYER__H

#include <QFileSystemWatcher>
#include <QPlainTextEdit>
#include <memory>

namespace Doc {
class Document;
}

class MarkdownHighlighter;
// class QFileSystemWatcher;

// namespace Ui {
QT_BEGIN_NAMESPACE
namespace Ui {
class DocumentContentView;
}
QT_END_NAMESPACE

class DocumentContentView : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit DocumentContentView(QWidget* parent = nullptr);
    ~DocumentContentView() override = default;

public slots:
    void setDocument(Doc::Document* document);
    void onDocumentDeleted(Doc::Document* document);
    void reset();

private:
    void onFileChanged(const QString& path);
    void display();

private:
    Doc::Document* mDocument;
    std::unique_ptr<MarkdownHighlighter> mHighlighter;
    std::unique_ptr<QFileSystemWatcher> mWatcher;
};
// }   // namespace Ui

#endif   // DOCUMENTDISPLAYER__H
