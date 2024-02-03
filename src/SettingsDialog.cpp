#include "SettingsDialog.h"
#include "./ui_SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    ui->lineSeparatorGroupBox->setHidden(true);   // TODO: unhide this and properly implement it
}


SettingsDialog::~SettingsDialog() {
    delete ui;
}


DataDirectoryCentralWidget* SettingsDialog::getDataDirWidget() {
    return ui->dataDirWidget;
}


auto* SettingsDialog::externalMdReaders() {
    return ui->mdReaders;
}


void SettingsDialog::accept() {
    ui->mdReaders->accept();
    ui->dataDirWidget->accept();
    QDialog::accept();
}
