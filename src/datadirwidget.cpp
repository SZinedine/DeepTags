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
#include "datadirwidget.h"
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include "settings.h"
#include "ui_datadirwidget.h"

DataDirWidget::DataDirWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::DataDirWidget), dataDirectoryStr("") {
    ui->setupUi(this);
    ui->m_directory->setText(path(false));
    connect(ui->m_browse, &QPushButton::clicked, this, &DataDirWidget::browse);
}

DataDirWidget::~DataDirWidget() {
    delete ui;
}

void DataDirWidget::browse() {
    auto dir = QFileDialog::getExistingDirectory(this, tr("Data Directory"), path(true));
    if (!dir.isEmpty()) ui->m_directory->setText(dir);
}

QString DataDirWidget::path(bool substitute) {
    QString current;
    if (Settings::dataDirectoryIsSet()) {
        current          = Settings::dataDirectory();
        dataDirectoryStr = current;
    } else {
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }

    if (substitute)
        return (current.isEmpty()) ? QDir::homePath() : current;
    else
        return current;
}

void DataDirWidget::accept() {
    QString dir = ui->m_directory->text();
    if (dir == dataDirectoryStr) return;   // if it hadn't changed
    if (!QDir().exists(dir)) {
        bool ok = QDir().mkdir(dir);
        if (!ok) {
            QMessageBox::warning(this, tr("error"),
                                 tr("Error occured when trying to create the specified folder"));
            return;
        }
    }
    Settings::setDataDirectory(dir);
    emit dataDirectoryChanged();
}
