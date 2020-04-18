#include "elementdialog.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QKeySequence>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include "settings.h"

ElementDialog::ElementDialog(QWidget* parent) : QDialog(parent), m_element(nullptr) {
    setup_forNewFile();
}


ElementDialog::ElementDialog(Element* element, QWidget* parent)
    : QDialog(parent), m_element(element) {
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

void ElementDialog::setup(bool visiblePath) {
    setFixedSize(450, 350);
    setModal(true);
    setWindowTitle(tr("Element Dialog"));

    auto* layout = new QFormLayout(this);
    setLayout(layout);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignLeft);

    m_path = new QLineEdit(this);
    m_path->setVisible(visiblePath);
    m_title     = new QLineEdit(this);
    m_pinned    = new QCheckBox(this);
    m_favorited = new QCheckBox(this);
    m_tags      = new TagsWidget(this);
    m_addTag    = new QPushButton(QIcon(":images/add.png"), "", this);
    m_delTag    = new QPushButton(QIcon(":images/delete.png"), "", this);
    m_addTag->setToolTip(tr("Add a tag"));
    m_delTag->setToolTip(tr("Delete a tag"));
    connect(m_addTag, &QPushButton::clicked, m_tags, [=] {
        m_tags->setFocus();
        m_tags->add();
    });
    connect(m_delTag, &QPushButton::clicked, m_tags, [=] { m_tags->del(); });
    m_addTag->setMaximumWidth(35);
    m_delTag->setMaximumWidth(35);
    auto addDelButtonsLayout = new QVBoxLayout;
    addDelButtonsLayout->addWidget(m_addTag, 0, Qt::AlignTop);
    addDelButtonsLayout->addWidget(m_delTag, 0, Qt::AlignTop);
    addDelButtonsLayout->addStretch();
    addDelButtonsLayout->setContentsMargins(0, 0, 0, 0);

    auto tagsLayout = new QHBoxLayout;
    tagsLayout->addWidget(m_tags);
    tagsLayout->addLayout(addDelButtonsLayout);
    tagsLayout->setContentsMargins(0, 0, 0, 0);

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    if (visiblePath) layout->addRow(tr("File Path: "), m_path);
    layout->addRow(tr("Title: "), m_title);
    layout->addRow(tr("Pinned: "), m_pinned);
    layout->addRow(tr("Favorite: "), m_favorited);
    layout->addRow(tr("Tags: "), tagsLayout);
    layout->addRow(buttons);

    setupKeyboard();
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
    setup(true);
    m_title->setText(m_element->title());
    m_path->setText(m_element->path());
    m_path->setReadOnly(true);
    m_pinned->setChecked(m_element->pinned());
    m_favorited->setChecked(m_element->favorited());
    m_tags->setTags(be::getUnparsedTags(m_element->getHeader()));
    connect(buttons, &QDialogButtonBox::accepted, this, [=]() { accept_(); });
}


void ElementDialog::setup_forNewFile() {
    setup(false);
    m_title->setText(tr("Untitled"));
    m_path->setVisible(false);
    connect(buttons, &QDialogButtonBox::accepted, this, [=]() { save(); });
}


void ElementDialog::save() {
    if (title().isEmpty()) {
        QMessageBox::warning(this, tr("Title isn't set"),
                             tr("You have to set at least the title to save the file"));
        return;
    }

    const QString dataDir = Settings::dataDirectory();
    QString filename;
    int n = 0;

start:
    filename = m_title->text();
    if (n != 0) filename += " (" + QString::number(n) + ")";
    formatFilename(filename);
    filename = dataDir + "/" + filename;
    n++;

    if (QFile::exists(filename)) goto start;

    // m_path = new QLineEdit(this);
    m_path->setText(filename);

    be::createNewFile(filename, title());
    auto e = new Element(filename);
    if (pinned()) e->addPinnedLine(true);
    if (favorited()) e->addFavoritedLine(true);
    const StringList t = tags();
    if (!t.empty()) e->addTagsLine(t);

    accept();
}


StringList ElementDialog::tags() const {
    return m_tags->tags();
}

void ElementDialog::formatFilename(QString& str) {
    str = str.simplified();
    if (str.isEmpty()) return;
    auto hasMdExtension = [](const QString& s) -> bool {
        QStringList ex{ ".md", ".markdown", ".mdown", ".mrkdn" };
        for (auto& e : ex)
            if (s.endsWith(e, Qt::CaseInsensitive)) return true;
        return false;
    };
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
    int n = 0;

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
    m_element->setPath(newFilePath);
    accept();
}


/*********** TagsWidget **************/

TagsWidget::TagsWidget(QWidget* parent) : QListWidget(parent) {
    connect(this, &TagsWidget::itemDoubleClicked, this, &TagsWidget::persistentEditor);
    connect(this, &TagsWidget::itemSelectionChanged, this, &TagsWidget::closeAllPersistentEditors);
    connect((new QShortcut(QKeySequence("Return"), this)), &QShortcut::activated, this,
            &TagsWidget::persistentEditor);
}


TagsWidget::TagsWidget(StringList labels, QWidget* parent) : TagsWidget(parent) {
    setTags(labels);
}

void TagsWidget::closeAllPersistentEditors() {
    for (auto i = 0; i < count(); i++) closePersistentEditor(item(i));
}

void TagsWidget::add() {
    closeAllPersistentEditors();
    auto i = new QListWidgetItem(this);
    addItem(i);
    setCurrentItem(i);
    openPersistentEditor(i);
    editItem(i);
}

StringList TagsWidget::tags() const {
    StringList s;
    for (int i = 0; i < count(); i++) {
        auto it = item(i)->text().simplified();
        if (!it.isEmpty()) s.push_back(it);
    }
    return s;
}

void TagsWidget::setTags(StringList tags) {
    for (auto s : tags) addItem(s);
}

void TagsWidget::del() {
    int row = currentRow();
    if (item(row)) delete takeItem(row);
}

void TagsWidget::persistentEditor() {
    auto it = currentItem();
    if (!it) return;
        // QListWidget::isPersistentEditorOpen exists only since Qt 5.10
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    if (isPersistentEditorOpen(it))
        closePersistentEditor(it);
    else {
        openPersistentEditor(currentItem());
        editItem(currentItem());
    }
#else
    openPersistentEditor(currentItem());
    editItem(currentItem());
#endif
}
