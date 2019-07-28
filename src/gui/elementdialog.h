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
 * 	 * edit the information of an already existing file
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

    void setListOfTags();
    /**
     * Get info
     */
    inline std::string title() 	const	{	return m_title->text().simplified().toStdString(); 	}
    inline bool pinned()	const		{	return m_pinned->isChecked();		}
    inline bool favorited()	const 		{	return m_favorited->isChecked();	}
    StringList tags() const;
    inline Element* element() 			{	return m_element;					}
    inline std::string path()			{	return m_path->text().toStdString();}

private:
    void setup_forEditFile();
    void setup_forNewFile();
    /**
     * create an element and assign to it all the necessary information
     */
    void save();
    QString getLastDir() const;

    Element* m_element;
    QDialogButtonBox* buttons;

    QLineEdit* m_title;
    QLineEdit* m_path;
    QCheckBox* m_pinned;
    QCheckBox* m_favorited;
    QTextEdit* m_tags;
};

#endif // ElementDialog_H
