#include "datadirwidget.h"
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include "settings.h"
#include "ui_datadirwidget.h"

DataDirWidget::DataDirWidget(QWidget* parent) : QWidget(parent), ui(new Ui::DataDirWidget) {
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
    if (Settings::dataDirectoryIsSet())
        current = Settings::dataDirectory();
    else
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    if (substitute)
        return (current.isEmpty()) ? QDir::homePath() : current;
    else
        return current;
}

void DataDirWidget::accept() {
    QString dir = ui->m_directory->text();
    if (!QDir().exists(dir)) {
        bool ok = QDir().mkdir(dir);
        if (!ok) {
            QMessageBox::warning(this, tr("error"),
                                 tr("Error occured when trying to create the specified folder"));
            return;
        }
    }
    Settings::setDataDirectory(dir);
}
