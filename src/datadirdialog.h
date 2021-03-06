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
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DataDirDialog;
}
QT_END_NAMESPACE

class DataDirDialog : public QDialog {
    Q_OBJECT
public:
    explicit DataDirDialog(QWidget* parent = nullptr);
    ~DataDirDialog();
    void accept() override;
signals:
    void dataDirectoryChanged();

private:
    Ui::DataDirDialog* ui;
};
