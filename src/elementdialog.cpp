/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2022  Zineddine Saibi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/
#include "elementdialog.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QKeySequence>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include "settings.h"
#include "ui_elementdialog.h"

ElementDialog::ElementDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ElementDialog), m_element(nullptr) {
    ui->setupUi(this);
    setup(false);
}

ElementDialog::ElementDialog(Element* element, QWidget* parent)
    : QDialog(parent), ui(new Ui::ElementDialog), m_element(element) {
    ui->setupUi(this);
    setup(true);
}

ElementDialog::~ElementDialog() {
    delete ui;
}

void ElementDialog::setup(bool visiblePath) {
    if (visiblePath) {   // editing and existing file
        ui->m_title->setText(m_element->title());
        ui->m_path->setText(m_element->path());
        ui->m_pinned->setChecked(m_element->pinned());
        ui->m_favorited->setChecked(m_element->favorited());
        ui->m_tags->setTags(be::getUnparsedTags(m_element->getHeader()));
        connect(ui->buttons, &QDialogButtonBox::accepted, this, [=]() { accept_(); });
    } else {   // hide the path LineEdit when we want to create a file
        ui->m_path->setVisible(false);
        ui->filePathLabel->setVisible(false);
        ui->m_title->setText(tr("Untitled"));
        ui->m_path->setVisible(false);
        connect(ui->buttons, &QDialogButtonBox::accepted, this, [=]() { save(); });
    }
    connect(ui->m_addTag, &QPushButton::clicked, ui->m_tags, [=] {
        ui->m_tags->setFocus();
        ui->m_tags->add();
    });
    connect(ui->m_delTag, &QPushButton::clicked, ui->m_tags, [=] { ui->m_tags->del(); });
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &ElementDialog::reject);

    setupKeyboard();
}

void ElementDialog::setupKeyboard() {
    auto t = new QShortcut(QKeySequence("Ctrl+t"), this);
    auto g = new QShortcut(QKeySequence("Ctrl+g"), this);
    auto p = new QShortcut(QKeySequence("Ctrl+p"), this);
    auto f = new QShortcut(QKeySequence("Ctrl+f"), this);
    connect(t, &QShortcut::activated, this, [=] { ui->m_title->setFocus(); });
    connect(g, &QShortcut::activated, this, [=] { ui->m_tags->setFocus(); });
    connect(p, &QShortcut::activated, this, [=] { ui->m_pinned->setChecked(!pinned()); });
    connect(f, &QShortcut::activated, this, [=] { ui->m_favorited->setChecked(!favorited()); });
}

void ElementDialog::save() {
    if (title().simplified().isEmpty()) {
        QMessageBox::warning(this, tr("Title isn't set"),
                             tr("You have to set at least the title to save the file"));
        return;
    }

    const QString dataDir = Settings::dataDirectory();
    QString filename;
    int n = 0;

    do {
        filename = ui->m_title->text();
        if (n != 0) filename += " (" + QString::number(n) + ")";
        formatFilename(filename);
        filename = dataDir + "/" + filename;
        n++;
    } while (QFile::exists(filename));

    ui->m_path->setText(filename);

    be::createNewFile(filename, title());
    auto e = new Element(filename);
    if (pinned()) e->addPinnedLine(true);
    if (favorited()) e->addFavoritedLine(true);
    const StringList t = tags();
    if (!t.empty()) e->addTagsLine(t);

    accept();
}

StringList ElementDialog::tags() const {
    return ui->m_tags->tags();
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
    if (ui->m_title->text().simplified().isEmpty()) {
        QMessageBox::warning(this, tr("Title Empty"),
                             tr("The title isn't set. It cannot be empty.") + QString("\t"));
        return;
    }
    QString currentCompletePath = ui->m_path->text();
    QString newFilePath;
    int n = 0;   // a number to place at the end of a filename in case it already exists

    do {
        newFilePath = ui->m_title->text();
        if (n != 0) newFilePath += " (" + QString::number(n) + ")";
        formatFilename(newFilePath);
        newFilePath = QFileInfo(currentCompletePath).absolutePath() + "/" + newFilePath;
        n++;
    } while (QFile::exists(newFilePath) && currentCompletePath != newFilePath);

    if (currentCompletePath == newFilePath) accept();
    QFile::rename(currentCompletePath, newFilePath);
    ui->m_path->setText(newFilePath);
    m_element->setPath(newFilePath);
    accept();
}

QString ElementDialog::title() const {
    return ui->m_title->text().simplified();
}

bool ElementDialog::pinned() const {
    return ui->m_pinned->isChecked();
}

bool ElementDialog::favorited() const {
    return ui->m_favorited->isChecked();
}

Element* ElementDialog::element() {
    return m_element;
}

QString ElementDialog::path() {
    return ui->m_path->text();
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
