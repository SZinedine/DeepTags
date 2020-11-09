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
#ifndef DATADIRWIDGET_H
#define DATADIRWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DataDirWidget;
}
QT_END_NAMESPACE

class DataDirWidget : public QWidget {
    Q_OBJECT

public:
    DataDirWidget(QWidget* parent = nullptr);
    ~DataDirWidget();
    void browse();
    QString path(bool substitute);
    void accept();
signals:
    void dataDirectoryChanged();

private:
    Ui::DataDirWidget* ui;
    QString dataDirectoryStr;   // the data directory at the moment the widget is created
};
#endif   // DATADIRWIDGET_H
