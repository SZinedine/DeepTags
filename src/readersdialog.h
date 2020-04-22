/*************************************************************************
 * DeepTags, Markdown Notes Manager
 * Copyright (C) 2020  Zineddine Saibi
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
#ifndef READERSDIALOG_H
#define READERSDIALOG_H

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;
class QListWidget;

class ReadersDialog : public QDialog {
    Q_OBJECT
public:
    ReadersDialog(QWidget* parent = nullptr);
    void setup();
    void accept_();
    void addItem();
    void delItem();
    void itemUp();
    void itemDown();
    void browse();
    bool added(const QString& ed);

private:
    QListWidget* listWidget;
    QLineEdit* editorLine;
    QDialogButtonBox* validateDialog;
};

#endif
