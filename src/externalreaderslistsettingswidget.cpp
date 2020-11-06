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
#include "externalreaderslistsettingswidget.h"
#include "ui_externalreaderslistsettingswidget.h"
#include "settings.h"
#include <QShortcut>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

ExternalReadersListSettingsWidget::ExternalReadersListSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ExternalReadersListSettingsWidget) {
    ui->setupUi(this);
    ui->listWidget->addItems(Settings::mdEditors());

    // connect(ui->validateDialog, &QDialogButtonBox::accepted, this, &ExternalReadersListSettingsWidget::accept);
    // connect(ui->validateDialog, &QDialogButtonBox::rejected, this, &ExternalReadersListSettingsWidget::reject);
    connect(ui->addLine, &QPushButton::clicked, this, &ExternalReadersListSettingsWidget::addItem);
    connect(ui->rmButton, &QPushButton::clicked, this, &ExternalReadersListSettingsWidget::delItem);
    connect(ui->upButton, &QPushButton::clicked, this, &ExternalReadersListSettingsWidget::itemUp);
    connect(ui->downButton, &QPushButton::clicked, this, &ExternalReadersListSettingsWidget::itemDown);
    connect((new QShortcut(QKeySequence("Return"), this)), &QShortcut::activated, this, &ExternalReadersListSettingsWidget::addItem);
}

ExternalReadersListSettingsWidget::~ExternalReadersListSettingsWidget() { delete ui; }

void ExternalReadersListSettingsWidget::accept() {
    QStringList lst;
    for (int i = 0; i < ui->listWidget->count(); i++) lst.push_back(ui->listWidget->item(i)->text());
    Settings::saveEditors(lst);
    emit accepted();
}


void ExternalReadersListSettingsWidget::addItem() {
    QString editor = ui->editorLine->text().simplified();
    if (editor.isEmpty()) return;
    if (added(editor)) {
        ui->editorLine->clear();
        return;
    }
    ui->listWidget->addItem(new QListWidgetItem(editor, ui->listWidget));
    ui->editorLine->clear();
}

bool ExternalReadersListSettingsWidget::added(const QString& ed) {
    for (int i = 0; i < ui->listWidget->count(); i++)
        if (ui->listWidget->item(i)->text() == ed) return true;
    return false;
}

void ExternalReadersListSettingsWidget::delItem() {
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (!item) return;
    auto answer =
        QMessageBox::question(this, tr("confirmation"), tr("Do you really want to delete it?"));
    if (answer == QMessageBox::No) return;
    delete item;
}

void ExternalReadersListSettingsWidget::itemUp() {
    int row   = ui->listWidget->currentRow();
    auto item = ui->listWidget->takeItem(row);
    if (!item) return;
    ui->listWidget->insertItem(row - 1, item);
    ui->listWidget->setCurrentItem(item);
}


void ExternalReadersListSettingsWidget::itemDown() {
    int row   = ui->listWidget->currentRow();
    auto item = ui->listWidget->takeItem(row);
    if (!item) return;
    ui->listWidget->insertItem(row + 1, item);
    ui->listWidget->setCurrentItem(item);
}

void ExternalReadersListSettingsWidget::browse() {
    auto f = QFileDialog::getOpenFileName(this, tr("Markdown Editor"), QDir::homePath());
    f      = f.simplified();
    if (f.isEmpty()) return;
    ui->editorLine->setText(f);
}

