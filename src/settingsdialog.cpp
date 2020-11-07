#include "settingsdialog.h"
#include <QButtonGroup>
#include "ui_settingsdialog.h"


SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    exec();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}


void SettingsDialog::accept() {
    ui->mdReaders->accept();
    ui->dataDirWidget->accept();
    QDialog::accept();
}
