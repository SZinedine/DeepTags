#ifndef DATADIRECTORYCENTRALWIDGET__H
#define DATADIRECTORYCENTRALWIDGET__H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DataDirectoryCentralWidget;
}
QT_END_NAMESPACE

class DataDirectoryCentralWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataDirectoryCentralWidget(QWidget* parent = nullptr);
    ~DataDirectoryCentralWidget() override;
    /**
     * return the Data Directory from the Settings,
     * otherwise the standard path of Document
     */
    static QString getDataDir();

public slots:
    /**
     * save the data directory into the settings if it was changed
     * if the directory doesn't exist, it will be created
     */
    bool accept();

private slots:
    void browse();

signals:
    void dataDirectoryChanged();

private:
    Ui::DataDirectoryCentralWidget* ui;
};

#endif   // DATADIRECTORYCENTRALWIDGET__H
