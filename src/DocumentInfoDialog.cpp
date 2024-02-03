#include "DocumentInfoDialog.h"
#include <QStringListModel>
#include "./ui_DocumentInfoDialog.h"
#include "Document.h"

DocumentInfoDialog::DocumentInfoDialog(Doc::Document* document, QWidget* parent)
    : QDialog(parent), ui(new Ui::DocumentInfoDialog), mDocument{ document },
      mTagModel(std::make_unique<QStringListModel>()) {
    ui->setupUi(this);
    ui->mTags->setModel(mTagModel.get());

    if (document != nullptr) {
        mDocument = document;
        editModeSetup();
    } else {
        newFileModeSetup();
    }

    setupSignalsAndSlots();
}


DocumentInfoDialog::~DocumentInfoDialog() {
    delete ui;
}


void DocumentInfoDialog::newFileModeSetup() {
    ui->mPath->setVisible(false);   // TODO: should make it visible and show the title modifications
    ui->filePathLabel->setVisible(false);
    ui->mTitle->setText("New Document");
    ui->mPinned->setChecked(false);
    ui->mFavorited->setChecked(false);

    ui->mTitle->selectAll();
}


void DocumentInfoDialog::editModeSetup() {
    ui->mPath->setText(mDocument->getPath());
    ui->mTitle->setText(mDocument->getTitle());
    ui->mPinned->setChecked(mDocument->isPinned());
    ui->mFavorited->setChecked(mDocument->isFavorited());

    mTagModel->setStringList(mDocument->getTags().toList());
}

void DocumentInfoDialog::setupSignalsAndSlots() {
    connect(ui->buttons, &QDialogButtonBox::accepted, this, &DocumentInfoDialog::accept);
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &DocumentInfoDialog::reject);
    connect(ui->mAddTag, &QPushButton::clicked, this, &DocumentInfoDialog::addTagRow);
    connect(ui->mDeleteTag, &QPushButton::clicked, this, &DocumentInfoDialog::deleteTagRow);
}


bool DocumentInfoDialog::isFavorited() const {
    return ui->mFavorited->isChecked();
}


bool DocumentInfoDialog::isPinned() const {
    return ui->mPinned->isChecked();
}


QString DocumentInfoDialog::getTitle() const {
    return ui->mTitle->text().simplified();
}


QVector<QString> DocumentInfoDialog::getTags() const {
    return mTagModel->stringList().toVector();
}


void DocumentInfoDialog::addTagRow() {
    QStringList sl = mTagModel->stringList();
    sl << "";

    mTagModel->setStringList(sl);
    auto index = mTagModel->index(sl.size() - 1, 0);
    ui->mTags->scrollToBottom();
    ui->mTags->setCurrentIndex(index);
    ui->mTags->edit(index);
}


void DocumentInfoDialog::deleteTagRow() {
    QStringList sl = mTagModel->stringList();
    int row        = ui->mTags->currentIndex().row();
    if (row < sl.size() && 0 <= row) {
        sl.removeAt(row);
        mTagModel->setStringList(sl);
    }
}
