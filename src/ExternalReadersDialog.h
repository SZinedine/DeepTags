#ifndef EXTERNALREADERSDIALOG__H
#define EXTERNALREADERSDIALOG__H


#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ExternalReadersDialog;
}
QT_END_NAMESPACE

class ExternalReadersDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExternalReadersDialog(QWidget* parent = nullptr);
    ~ExternalReadersDialog() override;

public slots:
    void accept() final;

private:
    Ui::ExternalReadersDialog* ui;
};

#endif   // EXTERNALREADERSDIALOG__H
