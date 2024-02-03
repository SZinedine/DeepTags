#include "ExternalReadersDialog.h"
#include "ExternalReadersCentralWidget.h"
#include "ui_ExternalReadersDialog.h"

ExternalReadersDialog::ExternalReadersDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ExternalReadersDialog) {
    ui->setupUi(this);
    exec();
}

ExternalReadersDialog::~ExternalReadersDialog() {
    delete ui;
}

void ExternalReadersDialog::accept() {
    ui->externalReadersCentralWidget->accept();
    QDialog::accept();
}
