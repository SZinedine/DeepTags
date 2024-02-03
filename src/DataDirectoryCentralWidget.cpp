#include "DataDirectoryCentralWidget.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include "Settings.h"
#include "ui_DataDirectoryCentralWidget.h"


DataDirectoryCentralWidget::DataDirectoryCentralWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::DataDirectoryCentralWidget) {
    ui->setupUi(this);
    ui->mDirectory->setText(getDataDir());
    connect(ui->mBrowse, &QPushButton::clicked, this, &DataDirectoryCentralWidget::browse);
}


DataDirectoryCentralWidget::~DataDirectoryCentralWidget() {
    delete ui;
}


QString DataDirectoryCentralWidget::getDataDir() {
    QString dataDir = Ui::Settings::loadDataDirectory();
    if (dataDir.isEmpty()) {
        // "~/Documents/Notes" OR "C:/Users/<USER>/Documents/Notes". TODO: test this on Windows
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + "Notes";
    }

    return dataDir;
}


bool DataDirectoryCentralWidget::accept() {
    const QString oldDataDirectory = Ui::Settings::loadDataDirectory().simplified();
    const QString newDataDirectory = ui->mDirectory->text().simplified();

    if (oldDataDirectory == newDataDirectory) {
        return false;
    }

    if (!QDir().exists(newDataDirectory)) {
        if (!QDir().mkdir(newDataDirectory)) {
            QMessageBox::critical(this, tr("error"),
                                  tr("Error while trying to create the Data Directory"));
            return false;
        }
    }

    Ui::Settings::saveDataDirectory(newDataDirectory);
    emit dataDirectoryChanged();

    return true;
}


void DataDirectoryCentralWidget::browse() {
    auto dir = QFileDialog::getExistingDirectory(this, tr("Data Directory"), getDataDir());

    if (!dir.isEmpty()) {
        ui->mDirectory->setText(dir);
    }
}
