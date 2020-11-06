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
#ifndef EXTERNALREADERSDIALOG_H
#define EXTERNALREADERSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ExternalReadersDialog; }
QT_END_NAMESPACE

class ExternalReadersDialog : public QDialog
{
    Q_OBJECT

public:
    ExternalReadersDialog(QWidget *parent = nullptr);
    ~ExternalReadersDialog();
    void accept() override;

private:
    Ui::ExternalReadersDialog *ui;
};
#endif // EXTERNALREADERSDIALOG_H
