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

class QLineEdit;
class QDialogButtonBox;

class DataDirDialog : public QDialog {
    Q_OBJECT
public:
    explicit DataDirDialog(QWidget* parent = nullptr);
    void accept() override;

private:
    void setup();
    void browse();
    QString path(bool substitute);

private:
    QLineEdit* m_directory;
    QPushButton* m_browse;
    QDialogButtonBox* m_buttons;
};
