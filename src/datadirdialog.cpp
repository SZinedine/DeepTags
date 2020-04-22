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
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>
#include "settings.h"


DataDirDialog::DataDirDialog(QWidget* parent)
    : QDialog(parent), m_directory(nullptr), m_browse(nullptr), m_buttons(nullptr) {
    setFixedSize(QSize(450, 170));
    setup();
    setWindowTitle(tr("Set Data Directory"));
}


void DataDirDialog::setup() {
    QString s_head;
    s_head.append(tr("Please choose a Data Directory where you want your notes to be saved. "));
    s_head.append(tr("If you already have notes in a folder, point it out to import them."));

    auto header = new QLabel(s_head);
    header->setWordWrap(true);
    header->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_directory = new QLineEdit(path(false));
    m_browse    = new QPushButton(tr("Browse"), this);
    m_buttons   = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &DataDirDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &DataDirDialog::reject);
    connect(m_browse, &QPushButton::clicked, this, &DataDirDialog::browse);

    // layout
    auto layout = new QVBoxLayout;
    layout->addWidget(header);
    auto directoryLayout = new QHBoxLayout;
    directoryLayout->setContentsMargins(0, 0, 0, 0);
    directoryLayout->addWidget(m_directory);
    directoryLayout->addWidget(m_browse);
    layout->addWidget(header);
    layout->addLayout(directoryLayout);
    layout->addWidget(m_buttons, Qt::AlignRight);
    setLayout(layout);
}

void DataDirDialog::browse() {
    auto dir = QFileDialog::getExistingDirectory(this, "Data Directory", path(true));
    if (!dir.isEmpty()) m_directory->setText(dir);
}

QString DataDirDialog::path(bool substitute) {
    auto current = (Settings::dataDirectoryIsSet())
                       ? Settings::dataDirectory()
                       : QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (substitute)
        return (current.isEmpty()) ? QDir::homePath() : current;
    else
        return current;
}

void DataDirDialog::accept() {
    QString dir = m_directory->text();
    if (!QDir().exists(dir)) {
        bool ok = QDir().mkdir(dir);
        if (!ok) {
            QMessageBox::warning(this, "error",
                                 "Error occured when trying to create the specified folder");
            return;
        }
    }
    Settings::setDataDirectory(dir);
    QDialog::accept();
}
