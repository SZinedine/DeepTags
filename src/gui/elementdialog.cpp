#include "elementdialog.h"
#include <QFormLayout>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

ElementDialog::ElementDialog(QWidget* parent)
    : QDialog(parent)
{
    setFixedSize(400, 300);
    setModal(true);
    m_element = nullptr;
    setup_forNewFile();
}


ElementDialog::ElementDialog(Element* element, QWidget* parent)
    : QDialog(parent)
{
    setFixedSize(400, 300);
    setModal(true);
    m_element = element;
    setup_forEditFile();
}


void ElementDialog::setup_forEditFile() {
    QFormLayout* layout = new QFormLayout;
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_title = new QLineEdit(QString::fromStdString(m_element->title()));
    m_path = new QLineEdit(QString::fromStdString( m_element->path().string() ));
    m_path->setEnabled(false);
    m_pinned = new QCheckBox;
    m_pinned->setChecked(m_element->pinned());
    m_favorited = new QCheckBox;
    m_favorited->setChecked(m_element->favorited());
    m_tags = new QTextEdit;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &ElementDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    layout->addRow("File Path: ", m_path);
    layout->addRow("Title: ", m_title);
    layout->addRow("Pinned: ", m_pinned);
    layout->addRow("Favorite: ", m_favorited);
    layout->addRow("Tags: ", m_tags);
    layout->addRow(buttons);

    setListOfTags();
}


void ElementDialog::setup_forNewFile() {
    QFormLayout* layout = new QFormLayout;
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_title = new QLineEdit(this);
    m_title->setText("Untitled");
    m_pinned = new QCheckBox;
    m_favorited = new QCheckBox;
    m_tags = new QTextEdit;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [=](){		save();		 });
    connect(buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    layout->addRow("Title: ", m_title);
    layout->addRow("Pinned: ", m_pinned);
    layout->addRow("Favorite: ", m_favorited);
    layout->addRow("Tags: ", m_tags);
    layout->addRow(buttons);
}


void ElementDialog::save() {
    if (title().empty()) {
        QMessageBox::warning(this, "Title didn't set", "You have to set at least the title to save the file");
        return;
    }

    QString default_ = getLastDir() + QString("/") + m_title->text().simplified();
    QString f = QFileDialog::getSaveFileName(this, "Save File", default_, "Text file (*.md *.MD *.markdown)");

    if (f.isEmpty()) return;
    m_path = new QLineEdit;
    m_path->setText(f);

    const fs::path fsPath(f.toStdString());
    Element::createNewFile(fsPath, title());
    Element* e = new Element(fsPath);
    if (pinned()) e->addPinnedLine(true);
    if (favorited()) e->addFavoritedLine(true);
    const StringList t = tags();
    if (!t.empty()) e->addTagsLine(t);

    accept();
}


void ElementDialog::setListOfTags() {
    auto header = Element::getHeader(m_element->path());
    auto lst = Element::extract_tags( Element::find_tags_inheader(header) );

    for (auto i : lst)
        m_tags->append(QString::fromStdString(i));
}


StringList ElementDialog::tags() const {
    QString raw = m_tags->toPlainText();
    QStringList lst = raw.split(QRegExp("\n"), QString::SkipEmptyParts);
    StringList res;
    for (const QString& i : lst) res.push_back(i.toStdString());
    return res;
}


QString ElementDialog::getLastDir() const {
    QSettings s;
    s.beginGroup("Main");
    QString d = s.value("lastOpenedDir").toString();
    s.endGroup();
    return (d.isEmpty()) ? QDir::homePath() : d;

}

