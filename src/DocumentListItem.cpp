#include "DocumentListItem.h"
#include "Document.h"

namespace Ui::Models {

DocumentListItem::DocumentListItem(Doc::Document* document) : mDocument{ document } {}


Doc::Document* DocumentListItem::getDocument() {
    return mDocument;
}


QString DocumentListItem::getTitle() const {
    return mDocument->getTitle();
}


QString DocumentListItem::getPath() const {
    return mDocument->getPath();
}


bool DocumentListItem::isPinned() const {
    return mDocument->isPinned();
}


bool DocumentListItem::isFavorited() const {
    return mDocument->isFavorited();
}


bool DocumentListItem::isDeleted() const {
    return mDocument->isDeleted();
}

}   // namespace Ui::Models
