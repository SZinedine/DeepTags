#include "elementdialog.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QKeySequence>
#include <QMessageBox>
#include <QRegExp>
#include <QShortcut>

#include "settings.h"

ElementDialog::ElementDialog(QWidget* parent) : QDialog(parent) {
    setFixedSize(400, 300);
    setModal(true);
    m_element = nullptr;
    setup_forNewFile();
    setupKeyboard();
}


ElementDialog::ElementDialog(Element* element, QWidget* parent) : QDialog(parent) {
    setFixedSize(400, 300);
    setModal(true);
    m_element = element;
    setup_forEditFile();
    setupKeyboard();
}

ElementDialog::~ElementDialog() {
    delete m_title;
    //    delete m_path;            // crash after quiting the app
    delete m_pinned;
    delete m_favorited;
    delete m_tags;
    delete buttons;
}

void ElementDialog::setupKeyboard() {
    auto t = new QShortcut(QKeySequence("Ctrl+t"), this);
    auto g = new QShortcut(QKeySequence("Ctrl+g"), this);
    auto p = new QShortcut(QKeySequence("Ctrl+p"), this);
    auto f = new QShortcut(QKeySequence("Ctrl+f"), this);
    connect(t, &QShortcut::activated, this, [=] { m_title->setFocus(); });
    connect(g, &QShortcut::activated, this, [=] { m_tags->setFocus(); });
    connect(p, &QShortcut::activated, this, [=] { m_pinned->setChecked(!pinned()); });
    connect(f, &QShortcut::activated, this, [=] { m_favorited->setChecked(!favorited()); });
}

void ElementDialog::setup_forEditFile() {
    auto* layout = new QFormLayout(this);
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_title = new QLineEdit(QString::fromStdString(m_element->title()));
    m_path  = new QLineEdit(QString::fromStdString(m_element->path().string()));
    m_path->setEnabled(false);
    m_pinned = new QCheckBox;
    m_pinned->setChecked(m_element->pinned());
    m_favorited = new QCheckBox;
    m_favorited->setChecked(m_element->favorited());
    m_tags = new QTextEdit;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [=]() { accept_(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    layout->addRow(tr("File Path: "), m_path);
    layout->addRow(tr("Title: "), m_title);
    layout->addRow(tr("Pinned: "), m_pinned);
    layout->addRow(tr("Favorite: "), m_favorited);
    layout->addRow(tr("Tags: "), m_tags);
    layout->addRow(buttons);

    setListOfTags();
}


void ElementDialog::setup_forNewFile() {
    auto* layout = new QFormLayout(this);
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_title = new QLineEdit(this);
    m_title->setText(tr("Untitled"));
    m_pinned    = new QCheckBox;
    m_favorited = new QCheckBox;
    m_tags      = new QTextEdit;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [=]() { save(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    layout->addRow(tr("Title: "), m_title);
    layout->addRow(tr("Pinned: "), m_pinned);
    layout->addRow(tr("Favorite: "), m_favorited);
    layout->addRow(tr("Tags: "), m_tags);
    layout->addRow(buttons);
}


void ElementDialog::save() {
    if (title().empty()) {
        QMessageBox::warning(this, tr("Title isn't set"),
                             tr("You have to set at least the title to save the file"));
        return;
    }

    const QString dataDir = Settings::dataDirectory();
    QString       filename;
    int           n = 0;

start:
    filename = m_title->text();
    if (n != 0) filename += " (" + QString::number(n) + ")";
    formatFilename(filename);
    filename = dataDir + "/" + filename;
    n++;

    if (QFile::exists(filename)) goto start;

    m_path = new QLineEdit;
    m_path->setText(filename);

    fs::path path(filename.toStdString().c_str());
    be::createNewFile(path, title());
    auto* e = new Element(path);
    if (pinned()) e->addPinnedLine(true);
    if (favorited()) e->addFavoritedLine(true);
    const StringList t = tags();
    if (!t.empty()) e->addTagsLine(t);

    accept();
}


void ElementDialog::setListOfTags() {
    auto lst = be::getUnparsedTags(m_element->path());

    for (const std::string& i : lst) m_tags->append(QString::fromStdString(i));
}


StringList ElementDialog::tags() const {
    QString     raw = m_tags->toPlainText();
    QStringList lst = raw.split(QRegExp("\n"), QString::SkipEmptyParts);
    StringList  res;
    for (const QString& i : lst) res.push_back(i.toStdString());
    return res;
}


void ElementDialog::formatFilename(QString& str) {
    str                 = str.simplified();
    auto hasMdExtension = [](const QString& s) -> bool {
        QStringList ex{".md", ".markdown", ".mdown", ".mrkdn"};
        for (auto& e : ex)
            if (s.endsWith(e, Qt::CaseInsensitive)) return true;
        return false;
    };
    if (str.isEmpty()) return;
    str.replace(":", ";");
    str.replace("/", "-");
    str.replace("|", "-");
    str.replace("<", "(");
    str.replace(">", ")");
    str.replace("*", "");
    str.replace("\\", "-");
    str.replace("?", "");
    if (!hasMdExtension(str)) str.append(".md");
}


void ElementDialog::accept_() {
    if (m_title->text().isEmpty()) {
        QMessageBox::warning(this, tr("Title Empty"),
                             tr("The title isn't set. It cannot be empty.") + QString("\t"));
        return;
    }
    QString currentCompletePath = m_path->text();
    QString newFilePath;
    int     n = 0;

start:
    newFilePath = m_title->text();
    if (n != 0) newFilePath += " (" + QString::number(n) + ")";
    formatFilename(newFilePath);
    newFilePath = QFileInfo(currentCompletePath).absolutePath() + "/" + newFilePath;
    n++;

    if (QFile::exists(newFilePath) && currentCompletePath != newFilePath) goto start;

    if (currentCompletePath == newFilePath) accept();
    QFile::rename(currentCompletePath, newFilePath);
    m_path->setText(newFilePath);
    m_element->setPath(fs::path(newFilePath.toStdString().c_str()));
    accept();
}
