#ifndef DOCUMENTLISTITEM__H
#define DOCUMENTLISTITEM__H

class QString;

namespace Doc {
class Document;
}

namespace Ui::Models {

class DocumentListItem final {
public:
    explicit DocumentListItem(Doc::Document* document);
    DocumentListItem(const DocumentListItem&)            = delete;
    DocumentListItem& operator=(const DocumentListItem&) = delete;
    DocumentListItem(DocumentListItem&&)                 = default;
    DocumentListItem& operator=(DocumentListItem&&)      = default;
    ~DocumentListItem()                                  = default;
    Doc::Document* getDocument();
    [[nodiscard]] QString getTitle() const;
    [[nodiscard]] QString getPath() const;
    [[nodiscard]] bool isPinned() const;
    [[nodiscard]] bool isFavorited() const;
    [[nodiscard]] bool isDeleted() const;

private:
    Doc::Document* mDocument;
};


}   // namespace Ui::Models
#endif   // DOCUMENTLISTITEM__H
