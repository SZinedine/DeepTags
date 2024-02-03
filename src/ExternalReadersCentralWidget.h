#ifndef EXTERNALREADERSCENTRALWIDGET__H
#define EXTERNALREADERSCENTRALWIDGET__H

#include <QWidget>

class QStringListModel;
class QShortcut;

QT_BEGIN_NAMESPACE
namespace Ui {
class ExternalReadersCentralWidget;
}
QT_END_NAMESPACE

class ExternalReadersCentralWidget : public QWidget {
    Q_OBJECT
public:
    explicit ExternalReadersCentralWidget(QWidget* parent = nullptr);
    ~ExternalReadersCentralWidget() override;
    // void browse(); // not used anywhere. TODO something with it or delete

public slots:
    void accept();

private:
    void addItem();
    void delItem();
    void itemUp();
    void itemDown();

signals:
    void accepted();

private:
    Ui::ExternalReadersCentralWidget* ui;
    std::unique_ptr<QStringListModel> mListViewModel;
    std::unique_ptr<QShortcut> mRetShortcut;
};

#endif   // EXTERNALREADERSCENTRALWIDGET__H
