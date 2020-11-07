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
#include "datadirdialog.h"
#include "datadirwidget.h"
#include "settings.h"
#include "ui_datadirdialog.h"

DataDirDialog::DataDirDialog(QWidget* parent) : QDialog(parent), ui(new Ui::DataDirDialog) {
    ui->setupUi(this);
}

DataDirDialog::~DataDirDialog() {
    delete ui;
}

void DataDirDialog::accept() {
    ui->dataDirWidget->accept();
    QDialog::accept();
}
