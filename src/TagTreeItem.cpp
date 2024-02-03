#include "TagTreeItem.h"
#include <QHash>
#include <QVariant>
#include "Settings.h"


namespace Ui::Models {

TagTreeItem::TagTreeItem(QString name, TagTreeItem* parent)
    : mName{ std::move(name) }, mParent{ parent } {
    if (mParent) {
        mParent->addChild(this);
    }
}


TagTreeItem::~TagTreeItem() {
    for (auto child : mChildren) {
        if (child) {
            delete child;
        }
    }
}


QString TagTreeItem::getName() const {
    return mName;
}


QString TagTreeItem::getCompleteName() const {
    QString result = mName;
    TagTreeItem* p = mParent;
    while (p) {
        result.prepend(p->getName() + "/");
        p = p->getParent();
    }

    return result.remove(0, 1);
}


QVector<TagTreeItem*> TagTreeItem::getChildren() {
    return mChildren;
}


TagTreeItem* TagTreeItem::getParent() {
    return mParent;
}

void TagTreeItem::addChild(TagTreeItem* child) {
    mChildren.push_back(child);
}


TagTreeItem* TagTreeItem::getChild(int index) {
    return (index < mChildren.size()) ? mChildren.at(index) : nullptr;
}


TagTreeItem* TagTreeItem::getChild(const QString& name) {
    for (TagTreeItem* item : mChildren) {
        if (name == item->mName) {
            return item;
        }
    }

    return nullptr;
}


void TagTreeItem::removeChild(const QString& name) {
    for (int i = 0; i < mChildren.size(); i++) {
        if (name == mChildren.at(i)->getName()) {
            delete mChildren.takeAt(i);
        }
    }
}


int TagTreeItem::row() const {
    if (mParent != nullptr) {
        return mParent->getChildren().indexOf(const_cast<TagTreeItem*>(this));
    }

    return -1;
}


void TagTreeItem::setName(const QString& name) {
    mName = name;
}


void TagTreeItem::setColor(const QString& color) const {
    if (color.isEmpty()) {
        return;
    }
    auto tag = getCompleteName();

    if (color == "default") {
        coloredTags.remove(tag);
    } else {
        coloredTags.insert(tag, QVariant::fromValue(color));
    }

    Settings::setColoredTags(coloredTags);

    refreshFromSettings();
}


void TagTreeItem::setPinned(bool pinned) {
    const QString name = getCompleteName();
    bool madeChange    = false;

    if (pinned) {
        if (!pinnedTags.contains(name)) {
            pinnedTags.push_back(name);
            madeChange = true;
        }

    } else {
        if (pinnedTags.contains(name)) {
            pinnedTags.removeAll(name);
            madeChange = true;
        }
    }

    if (madeChange) {
        Settings::setPinnedTags(pinnedTags);
    }
}


void TagTreeItem::sortChildren() {
    std::sort(mChildren.begin(), mChildren.end(), [](TagTreeItem* a, TagTreeItem* b) {
        if (a->isSpecialTag() && !b->isSpecialTag()) {
            return true;
        }

        if (!a->isSpecialTag() && b->isSpecialTag()) {
            return false;
        }

        if (a->isPinned() && !b->isPinned()) {
            return true;
        }

        if (!a->isPinned() && b->isPinned()) {
            return false;
        }

        return *a < *b;
    });

    for (auto& child : mChildren) {
        child->sortChildren();
    }
}


bool TagTreeItem::isSpecialTag() const {
    return specialTags.contains(getCompleteName());
}


bool TagTreeItem::isPinned() const {
    return pinnedTags.contains(getCompleteName());
}


QString TagTreeItem::getColor() const {
    return coloredTags[getCompleteName()].toString();
}


void TagTreeItem::refreshFromSettings() {
    pinnedTags  = Settings::getPinnedTags();
    coloredTags = Settings::getColoredTags();
}


/**
 *** Operator overloading
 */

bool operator<(const TagTreeItem& lhs, const TagTreeItem& rhs) {
    return lhs.mName < rhs.mName;
}


bool operator>(const TagTreeItem& lhs, const TagTreeItem& rhs) {
    return rhs < lhs;
}


bool operator<=(const TagTreeItem& lhs, const TagTreeItem& rhs) {
    return !(lhs > rhs);
}


bool operator>=(const TagTreeItem& lhs, const TagTreeItem& rhs) {
    return !(lhs < rhs);
}

}   // namespace Ui::Models
