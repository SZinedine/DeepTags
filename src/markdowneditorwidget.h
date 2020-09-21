#include <qmarkdowntextedit.h>

class MarkdownEditorWidget : public QMarkdownTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditorWidget(QWidget* parent=nullptr);
    
};

