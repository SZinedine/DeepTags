#include "elementdialog.h"
#include <QFormLayout>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include "settings.h"

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

ElementDialog::~ElementDialog() {
    delete m_title;
    delete m_path;
    delete m_pinned;
    delete m_favorited;
    delete m_tags;
    delete buttons;
}


void ElementDialog::setup_forEditFile() {
    auto *layout = new QFormLayout;
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
    connect(buttons, &QDialogButtonBox::accepted, this, [=](){  accept_();  });
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
    auto *layout = new QFormLayout;
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_title = new QLineEdit(this);
    m_title->setText(tr("Untitled"));
    m_pinned = new QCheckBox;
    m_favorited = new QCheckBox;
    m_tags = new QTextEdit;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [=](){  save();   });
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

    fs::path path( Settings::dataDirectory().toStdString().c_str() );
    path = path / fs::path(m_title->text().simplified().toStdString().c_str());
    path += ".md";

    if (fs::exists(path)) {
        QMessageBox::warning(this, tr("File already exist"), 
                QString(path.string().c_str()) + " already exists.");
        return;
    }
    
    m_path = new QLineEdit;
    m_path->setText(QString(path.c_str()));

    be::createNewFile(path, title());
    auto *e = new Element(path);
    if (pinned()) e->addPinnedLine(true);
    if (favorited()) e->addFavoritedLine(true);
    const StringList t = tags();
    if (!t.empty()) e->addTagsLine(t);

    accept();
}


void ElementDialog::setListOfTags() {
    auto header = be::getHeader(m_element->path());
    auto lst = be::extract_tags( be::find_tags_inheader(header) );

    for (const auto &i : lst)
        m_tags->append(QString::fromStdString(i));
}


StringList ElementDialog::tags() const {
    QString raw = m_tags->toPlainText();
    QStringList lst = raw.split(QRegExp("\n"), QString::SkipEmptyParts);
    StringList res;
    for (const QString& i : lst) res.push_back(i.toStdString());
    return res;
}




void ElementDialog::accept_() {
    if (m_title->text().isEmpty()){
        QMessageBox::warning(this, tr("Title Empty"),
                             tr("The title isn't set. It cannot be empty.")+QString("\t"));
        return;
    }
    accept();
}

