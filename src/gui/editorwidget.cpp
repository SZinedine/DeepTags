#include "editorwidget.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <iostream>

EditorWidget::EditorWidget(QWidget* parent)
    : QWidget(parent), m_currentPath(""), m_fileContent("") {
    auto layout = new QVBoxLayout;
    setLayout(layout);
    // m_toolBar = new QToolBar(this);
    m_editor = new QMarkdownTextEdit(this);
    // layout->addWidget(m_toolBar);
    layout->addWidget(m_editor);
    // m_toolBar->addAction("save", [&] { save(); });
    setVisible(false);
    m_editor->setReadOnly(true);

    connect(this, &EditorWidget::openedFile, this, [=] { setVisible(true); });
    connect(this, &EditorWidget::closedFile, this, [=] { setVisible(false); });
}


void EditorWidget::open(const QString& path) {
    closeFile();
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream strm(&f);
    m_fileContent = strm.readAll();
    m_editor->setText(m_fileContent);
    m_currentPath = path;
    emit openedFile(path);
    setVisible(true);
}

void EditorWidget::save() {
    QFile f(m_currentPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    QString new_text = m_editor->toPlainText();
    out << new_text;
    m_fileContent = new_text;
    emit savedFile(m_currentPath);
}

void EditorWidget::closeFile() {
    if (m_currentPath.isEmpty() || m_fileContent.isEmpty()) {
        return;
    }
    // bool changed = m_fileContent != m_editor->toPlainText();
    emit closedFile(m_currentPath);
    m_editor->clear();
    m_currentPath.clear();
    m_fileContent.clear();
}
