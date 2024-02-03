#include "DocumentContentView.h"
#include <3rdParty/qmarkdowntextedit/markdownhighlighter.h>
#include <QFileInfo>
#include "Document.h"

DocumentContentView::DocumentContentView(QWidget* parent)
    : QPlainTextEdit{ parent }, mDocument(nullptr),
      mHighlighter{ std::make_unique<MarkdownHighlighter>() },
      mWatcher{ std::make_unique<QFileSystemWatcher>() } {
    mHighlighter->setDocument(document());
    setReadOnly(true);
    connect(mWatcher.get(), &QFileSystemWatcher::fileChanged, this,
            &DocumentContentView::onFileChanged);
}


void DocumentContentView::setDocument(Doc::Document* document) {
    mDocument = document;
    display();
    mWatcher->addPath(mDocument->getPath());
}


void DocumentContentView::onDocumentDeleted(Doc::Document* document) {
    if (mDocument != nullptr && mDocument == document) {
        reset();
    }
}


void DocumentContentView::display() {
    if (mDocument != nullptr) {
        setPlainText(Doc::Utils::getFileContentAsString(mDocument->getPath()));
    }
}


void DocumentContentView::reset() {
    clear();

    if (const auto paths = mWatcher->files(); !paths.isEmpty()) {
        mWatcher->removePaths(paths);
    }

    mDocument = nullptr;
}


void DocumentContentView::onFileChanged(const QString& path) {
    if (!QFileInfo::exists(path)) {
        reset();
        return;
    }
    QTextCursor cur(textCursor());
    auto po = cur.position();
    display();
    cur.setPosition(po);
    setTextCursor(cur);
}
