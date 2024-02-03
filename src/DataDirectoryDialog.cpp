#include "DataDirectoryDialog.h"
#include "ui_DataDirectoryDialog.h"

DataDirectoryDialog::DataDirectoryDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::DataDirectoryDialog) {
    ui->setupUi(this);
    connect(ui->dataDirWidget, &DataDirectoryCentralWidget::dataDirectoryChanged, this,
            [this] { emit dataDirectoryChanged(); });
}


DataDirectoryDialog::~DataDirectoryDialog() {
    delete ui;
}


void DataDirectoryDialog::accept() {
    if (!ui->dataDirWidget->accept()) {
        return;
    }

    QDialog::accept();
}
