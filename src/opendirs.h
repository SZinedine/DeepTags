#ifndef OPENDIRS_H
#define OPENDIRS_H

/**
 *  Enter directory paths to open on every startup
 */

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

class OpenDirs : public QDialog
{
    Q_OBJECT
public:
    explicit OpenDirs(QDialog *parent=nullptr);
    void construct();
    static QString savedText();
    static QStringList savedTextList();

public slots:
    void save();
    void load();

private:
    QTextEdit* textWidget;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif // OPENDIRS_H
