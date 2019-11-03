#ifndef ElementDialog_H
#define ElementDialog_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include "../element/element.h"
#include <QDialogButtonBox>

/**
 * Dialog box that serves 2 purposes
 *   * create a new file (first constructor
 *      * edit the information of an already existing file
 */
class ElementDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief constructor for new file creation
     */
    ElementDialog(QWidget* parent=nullptr);
    /**
     * @brief constructor to edit existing Elements
     */
    ElementDialog(Element* element, QWidget* parent=nullptr);
    ~ElementDialog();

    void setListOfTags();
    /**
     * Get info
     */
    [[nodiscard]] inline std::string title() const { return m_title->text().simplified().toStdString(); }
    [[nodiscard]] inline bool pinned()       const { return m_pinned->isChecked();          }
    [[nodiscard]] inline bool favorited()    const { return m_favorited->isChecked();       }
    [[nodiscard]] StringList tags()          const;
    [[nodiscard]] inline Element* element()        { return m_element;                      }
    [[nodiscard]] inline std::string path()        { return m_path->text().toStdString();   }
    static void formatFilename(QString& str);

private:
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
    QTextEdit* m_tags;
};

#endif // ElementDialog_H
