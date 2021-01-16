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
#include "tagitem.h"
#include <QApplication>
#include <set>
#include "settings.h"

#define USER_TYPE_NUM 1500

TagItem::TagItem(const QString& label, QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent, QStringList(label), USER_TYPE_NUM), m_elements(new QList<Element*>()),
      m_special(false), m_pinned(false) {}

TagItem::TagItem(const QString& label, QTreeWidget* parent)
    : QTreeWidgetItem(parent, QStringList(label), USER_TYPE_NUM), m_elements(new QList<Element*>()),
      m_special(false), m_pinned(false) {}

TagItem::TagItem(const QString& label, const QString& icon, bool special, QTreeWidget* parent)
    : QTreeWidgetItem(parent, QStringList(label), USER_TYPE_NUM), m_elements(new QList<Element*>()),
      m_special(special), m_pinned(false) {
    if (!icon.isEmpty()) setIcon(0, QIcon(icon));
}

TagItem::TagItem(const TagItem& other) : QTreeWidgetItem(other) {
    m_special  = other.m_special;
    m_pinned   = other.m_pinned;
    m_elements = new QList<Element*>(*other.m_elements);
}

TagItem::TagItem(TagItem&& other) : QTreeWidgetItem(other) {
    m_special        = other.m_special;
    m_pinned         = other.m_pinned;
    m_elements       = other.m_elements;
    other.m_elements = nullptr;
}

TagItem& TagItem::operator=(TagItem& other) {
    m_special  = other.m_special;
    m_pinned   = other.m_pinned;
    m_elements = other.m_elements;
    return *this;
}

TagItem& TagItem::operator=(const TagItem& other) {
    m_special  = other.m_special;
    m_pinned   = other.m_pinned;
    m_elements = other.m_elements;
    return *this;
}

TagItem& TagItem::operator=(TagItem&& other) {
    m_special        = other.m_special;
    m_pinned         = other.m_pinned;
    m_elements       = other.m_elements;
    other.m_elements = nullptr;
    return *this;
}

TagItem::~TagItem() {
    if (m_elements) delete m_elements;
}


void TagItem::removeElement(Element* element) {
    for (int i = 0; i < m_elements->size(); i++)
        if (*m_elements->at(i) == *element) m_elements->removeAt(i);
}


bool TagItem::empty() const {
    if (!m_elements->empty()) return false;
    for (auto& child : children())
        if (!child->empty()) return false;
    return true;
}


bool TagItem::contains(Element* e) {
    if (m_elements->contains(e)) return true;
    for (auto& child : children())
        if (child->contains(e)) return true;
    return false;
}


void TagItem::setColor(const QString& color) {
    if (color.isEmpty())
        setForeground(0, qApp->palette().text());
    else {
        QBrush brush(foreground(0));
        brush.setColor(color);
        setForeground(0, brush);
    }
    Settings::setTagItemColor(label(), color);
}

void TagItem::setPinned(bool pinned) {
    m_pinned = pinned;
    QFont f  = font(0);
    f.setBold(pinned);
    setFont(0, f);
    if (pinned)
        Settings::setTagPinned(label());
    else
        Settings::setTagUnpinned(label());
}

QString TagItem::completeTag() {
    auto castIt = [](QTreeWidgetItem* it) { return (it) ? static_cast<TagItem*>(it) : nullptr; };
    TagItem* p  = castIt(parent());
    QString tag = label();
    while (p) {
        tag.prepend(p->label() + '/');
        p = castIt(p->parent());
    }
    return tag;
}

QList<Element*>* TagItem::allElements() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QSet<Element*> res(m_elements->begin(), m_elements->end());
#else
    QSet<Element*> res(m_elements->toSet());
#endif
    for (auto& c : children()) {
        for (auto& e : *c->allElements()) res += e;
    }
    auto lst = new QList<Element*>(res.values());
    return lst;
}

QVector<TagItem*> TagItem::children() const {
    QVector<TagItem*> v;
    for (int i = 0; i < childCount(); i++) v.push_back(static_cast<TagItem*>(child(i)));
    return v;
}
