#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace Ui::Models {
class TagTreeModel;
class DocumentListModel;
}   // namespace Ui::Models

namespace Doc {
class Document;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLineEdit;
class QMenu;
class QAction;
class QShortcut;
class QLabel;
class QSystemTrayIcon;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void closeEvent(QCloseEvent* event) final;
    void setup();
    void setupModels();
    void setupMenuBar();
    void setupSettings();
    void setupSignalsAndSlots();
    void setupMenu();
    void setupKeyboardShortcuts();
    void setupSystemTray();
    void loadDocuments();
    void clearDocuments();

private slots:
    void onTagClicked(const QStringList& tags);
    void toggleDocumentContentViewVisibility();
    void onChangeDataDirAction();
    void openFile(QString path, QString editor = "");
    void createNewFile();
    void settingsDialog();
    void search(const QString& text);
    void onNumberOfDocumentsChanged(int number);
    void onDocumentPermanentlyDeleted(Doc::Document* document);
    void onSystemTryIconActivated(int reason);
    void applyTheme(const QString& theme);
    void about();

signals:
    void loadedUi();

private:
    Ui::MainWindow* ui;
    QVector<Doc::Document*> mDocuments;
    std::unique_ptr<Ui::Models::TagTreeModel> mTagTreeModel;
    std::unique_ptr<Ui::Models::DocumentListModel> mDocumentListModel;
    std::unique_ptr<QLineEdit> mSearchBar;
    std::unique_ptr<QAction> mSearchBarEraseText;
    std::unique_ptr<QLabel> mNumberOfFilesLabel;
    std::unique_ptr<QShortcut> mSearchShortcut;
    std::unique_ptr<QSystemTrayIcon> mSystemTrayIcon;
    std::unique_ptr<QMenu> mSystemTrayMenu;
};
#endif   // MAINWINDOW_H
