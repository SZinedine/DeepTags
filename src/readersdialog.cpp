/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2020  Zineddine Saibi
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
#include "readersdialog.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include "settings.h"


ReadersDialog::ReadersDialog(QWidget* parent) : QDialog(parent) {
    setup();
    setFixedSize(400, 300);
    setModal(true);
    setWindowTitle("Markdown Readers Dialog");
    exec();
}

void ReadersDialog::setup() {
    editorLine   = new QLineEdit(this);
    auto addLine = new QPushButton(QIcon(":images/add.png"), "", this);
    // auto browsEditor = new QPushButton(this);

    listWidget = new QListWidget(this);
    listWidget->addItems(Settings::mdEditors());
    auto rmButton         = new QPushButton(QIcon(":images/delete.png"), "", this);
    auto upButton         = new QPushButton(QIcon(":images/arrowup.png"), "", this);
    auto downButton       = new QPushButton(QIcon(":images/arrowdown.png"), "", this);
    const int buttonWidth = 35;
    rmButton->setMaximumWidth(buttonWidth);
    upButton->setMaximumWidth(buttonWidth);
    downButton->setMaximumWidth(buttonWidth);

    validateDialog = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // layout
    auto editorLineLayout = new QHBoxLayout;
    editorLineLayout->addWidget(editorLine);
    editorLineLayout->addWidget(addLine);
    // editorLineLayout->addWidget(browsEditor);

    auto listLayout = new QGridLayout;
    listLayout->addWidget(listWidget, 0, 0, 5, 1);
    listLayout->addWidget(rmButton, 0, 1);
    listLayout->addWidget(upButton, 1, 1);
    listLayout->addWidget(downButton, 2, 1);

    auto layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Name of a Markdown editor/reader:"), this));
    layout->addLayout(editorLineLayout);
    layout->addWidget(new QLabel(tr("List of Markdown readers:"), this));
    layout->addLayout(listLayout);
    layout->addWidget(validateDialog);
    setLayout(layout);

    // signals and slots
    connect(validateDialog, &QDialogButtonBox::accepted, this, &ReadersDialog::accept_);
    connect(validateDialog, &QDialogButtonBox::rejected, this, &ReadersDialog::reject);
    connect(addLine, &QPushButton::clicked, this, &ReadersDialog::addItem);
    connect(rmButton, &QPushButton::clicked, this, &ReadersDialog::delItem);
    connect(upButton, &QPushButton::clicked, this, &ReadersDialog::itemUp);
    connect(downButton, &QPushButton::clicked, this, &ReadersDialog::itemDown);
    // connect(browsEditor, &QPushButton::clicked, this, &ReadersDialog::browse);
    connect((new QShortcut(QKeySequence("Return"), this)), &QShortcut::activated, this, [=] {
        if (editorLine->hasFocus())
            addItem();
        else
            accept_();
    });
}


void ReadersDialog::accept_() {
    QStringList lst;
    for (int i = 0; i < listWidget->count(); i++) lst.push_back(listWidget->item(i)->text());
    Settings::saveEditors(lst);
    accept();
}


void ReadersDialog::addItem() {
    QString editor = editorLine->text().simplified();
    if (editor.isEmpty()) return;
    if (added(editor)) {
        editorLine->clear();
        return;
    }
    listWidget->addItem(new QListWidgetItem(editor, listWidget));
    editorLine->clear();
}

bool ReadersDialog::added(const QString& ed) {
    for (int i = 0; i < listWidget->count(); i++)
        if (listWidget->item(i)->text() == ed) return true;
    return false;
}

void ReadersDialog::delItem() {
    QListWidgetItem* item = listWidget->currentItem();
    if (!item) return;
    auto answer = QMessageBox::question(this, "confirmation", "Do you really want to delete it?");
    if (answer == QMessageBox::No) return;
    delete item;
}

void ReadersDialog::itemUp() {
    int row   = listWidget->currentRow();
    auto item = listWidget->takeItem(row);
    if (!item) return;
    listWidget->insertItem(row - 1, item);
    listWidget->setCurrentItem(item);
}


void ReadersDialog::itemDown() {
    int row   = listWidget->currentRow();
    auto item = listWidget->takeItem(row);
    if (!item) return;
    listWidget->insertItem(row + 1, item);
    listWidget->setCurrentItem(item);
}

void ReadersDialog::browse() {
    auto f = QFileDialog::getOpenFileName(this, "Markdown Editor", QDir::homePath());
    f      = f.simplified();
    if (f.isEmpty()) return;
    editorLine->setText(f);
}
