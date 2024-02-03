#ifndef DOCUMENTINFODIALOG__H
#define DOCUMENTINFODIALOG__H

#include <QDialog>
#include <QStringListModel>

namespace Doc {
class Document;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class DocumentInfoDialog;
}
QT_END_NAMESPACE


class DocumentInfoDialog : public QDialog {
public:
    explicit DocumentInfoDialog(Doc::Document* document, QWidget* parent);
    ~DocumentInfoDialog() override;

    [[nodiscard]] bool isFavorited() const;
    [[nodiscard]] bool isPinned() const;
    [[nodiscard]] QString getTitle() const;
    [[nodiscard]] QVector<QString> getTags() const;

private:
    void editModeSetup();
    void newFileModeSetup();
    void setupSignalsAndSlots();
    void addTagRow();
    void deleteTagRow();

private:
    Ui::DocumentInfoDialog* ui;
    Doc::Document* mDocument;
    std::unique_ptr<QStringListModel> mTagModel;
};

#endif   // DOCUMENTINFODIALOG__H
