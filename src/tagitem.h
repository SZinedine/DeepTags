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
#ifndef TAGITEM_H
#define TAGITEM_H

#include <QTreeWidgetItem>
#include <QVector>
#include "element.h"

class TagItem : public QTreeWidgetItem {
public:
    TagItem(const QString& label, bool special = false, TagItem* parent = nullptr);
    TagItem(const QString& label, const QString& icon, bool special = false);
    ~TagItem() override;

    inline void addFiles(const QVector<Element*>& els) {
        for (const auto& i : els) addFile(i);
    }
    inline void addFile(Element* element) { m_elements->push_back(element); }
    inline void setLabel(const QString& label) { setText(0, label); }
    inline QVector<Element*>* elements() const { return m_elements; }
    inline QString label() const { return text(0); }
    inline int number() const { return m_elements->size(); }
    inline void remove(Element* e) { m_elements->removeAll(e); }
    inline bool empty() const { return m_elements->isEmpty(); }
    inline bool isSpecial() const { return m_special; }
    inline bool hasChildren() const { return (childCount() > 0); }
    bool contains(Element* e);
    void removeElement(Element* element);
    void setColor(const QString& color);
    bool pinned() const { return m_pinned; }
    void setPinned(bool pinned);
    QVector<Element*>* allElements() const;
    QVector<Element*> allElements_() const;
    QVector<TagItem*> children() const;

private:
    QVector<Element*>* m_elements;
    bool m_special;
    bool m_pinned;
};

#endif   // TAGITEM_H
