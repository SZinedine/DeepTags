#ifndef DATADIRECTORYDIALOG__H
#define DATADIRECTORYDIALOG__H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class DataDirectoryDialog;
}
QT_END_NAMESPACE


class DataDirectoryDialog : public QDialog {
    Q_OBJECT
public:
    explicit DataDirectoryDialog(QWidget* parent = nullptr);
    ~DataDirectoryDialog() override;
    void accept() final;

signals:
    void dataDirectoryChanged();

private:
    Ui::DataDirectoryDialog* ui;
};

#endif   // DATADIRECTORYDIALOG__H
