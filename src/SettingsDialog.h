#ifndef SETTINGSDIALOG__H
#define SETTINGSDIALOG__H

#include <QDialog>

class DataDirectoryCentralWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;
    DataDirectoryCentralWidget* getDataDirWidget();
    auto* externalMdReaders();
    void accept() final;

private:
    Ui::SettingsDialog* ui;
};

#endif   // SETTINGSDIALOG__H
