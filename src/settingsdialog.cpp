/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2021  Zineddine Saibi
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
#include "settingsdialog.h"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [=] { accept(); });
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::accept() {
    connect(ui->dataDirWidget, &DataDirWidget::dataDirectoryChanged,
            [=] { emit dataDirectoryChanged(); });
    ui->dataDirWidget->accept();
    ui->mdReaders->accept();
    disconnect(ui->dataDirWidget, nullptr, nullptr, nullptr);
    QDialog::accept();
}
