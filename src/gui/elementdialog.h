#ifndef ElementDialog_H
#define ElementDialog_H

#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include "../element/element.h"

class TagsWidget;
class QCheckBox;
class QDialogButtonBox;
class QPushButton;

/**
 * Dialog box that serves 2 purposes
 *   * create a new file (first constructor
 *      * edit the information of an already existing file
 */
class ElementDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * @brief constructor for new file creation
     */
    ElementDialog(QWidget* parent = nullptr);
    /**
     * @brief constructor to edit existing Elements
     */
    ElementDialog(Element* element, QWidget* parent = nullptr);
    ~ElementDialog() override;
    void setupKeyboard();
    /**
     * Get info
     */
    [[nodiscard]] inline QString title() const { return m_title->text().simplified(); }
    [[nodiscard]] inline bool pinned() const { return m_pinned->isChecked(); }
    [[nodiscard]] inline bool favorited() const { return m_favorited->isChecked(); }
    [[nodiscard]] StringList tags() const;
    [[nodiscard]] inline Element* element() { return m_element; }
    [[nodiscard]] inline QString path() { return m_path->text(); }
    static void formatFilename(QString& str);

private:
    void setup(bool visiblePath);
    void setup_forEditFile();
    void setup_forNewFile();
    /**
     * replaces accept() for the new file dialog
     * saves the necessary info beforehand
     */
    void save();
    /**
     * replaces accept() for the editing of files
     * purpose: check if the file string isn't empty before saving
     * and compose a filename from the title to rename the original file
     */
    void accept_();

    Element* m_element;
    QDialogButtonBox* buttons;
    QLineEdit* m_title;
    QLineEdit* m_path;
    QCheckBox* m_pinned;
    QCheckBox* m_favorited;
    TagsWidget* m_tags;
    QPushButton* m_addTag;
    QPushButton* m_delTag;
};

/**
 * the widget that contain the list of tags
 */
class TagsWidget : public QListWidget {
    Q_OBJECT
public:
    explicit TagsWidget(QWidget* parent);
    explicit TagsWidget(StringList labels, QWidget* parent);
    inline void add(QString label) { addItem(label); }
    void add();
    void del();
    void closeAllPersistentEditors();
    StringList tags() const;
    void setTags(StringList);
    void persistentEditor();
};

#endif   // ElementDialog_H
