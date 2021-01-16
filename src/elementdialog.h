/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2021  Zineddine Saibi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************/
#ifndef ElementDialog_H
#define ElementDialog_H

#include <QDialog>
#include <QListWidget>
#include "element.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ElementDialog;
}
QT_END_NAMESPACE

/**
 * Dialog box that serves 2 purposes
 *   * create a new file (first constructor
 *      * edit the information of an already existing file
 */
class ElementDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * constructor for new file creation
     */
    ElementDialog(QWidget* parent = nullptr);
    /**
     * constructor to edit existing Elements
     */
    ElementDialog(Element* element, QWidget* parent = nullptr);
    ~ElementDialog() override;
    void setupKeyboard();
    [[nodiscard]] QString title() const;
    [[nodiscard]] bool pinned() const;
    [[nodiscard]] bool favorited() const;
    [[nodiscard]] StringList tags() const;
    [[nodiscard]] Element* element();
    [[nodiscard]] QString path();
    static void formatFilename(QString& str);

private:
    void setup(bool visiblePath);
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

private:
    Ui::ElementDialog* ui;
    Element* m_element;
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
