#include "ExternalReadersCentralWidget.h"
#include <QFileDialog>
#include <QShortcut>
#include <QStringListModel>
#include "Settings.h"
#include "ui_ExternalReadersCentralWidget.h"


ExternalReadersCentralWidget::ExternalReadersCentralWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ExternalReadersCentralWidget) {
    ui->setupUi(this);

    mListViewModel = std::make_unique<QStringListModel>();
    mListViewModel->setStringList(Ui::Settings::loadExternalReaders());
    ui->listView->setModel(mListViewModel.get());

    connect(ui->addLine, &QPushButton::clicked, this, &ExternalReadersCentralWidget::addItem);
    connect(ui->rmButton, &QPushButton::clicked, this, &ExternalReadersCentralWidget::delItem);
    connect(ui->upButton, &QPushButton::clicked, this, &ExternalReadersCentralWidget::itemUp);
    connect(ui->downButton, &QPushButton::clicked, this, &ExternalReadersCentralWidget::itemDown);

    mRetShortcut = std::make_unique<QShortcut>(QKeySequence(QStringLiteral("Return")), this);
    connect(mRetShortcut.get(), &QShortcut::activated, this,
            &ExternalReadersCentralWidget::addItem);
}


ExternalReadersCentralWidget::~ExternalReadersCentralWidget() {
    delete ui;
}


void ExternalReadersCentralWidget::itemUp() {
    auto row = ui->listView->currentIndex().row();
    mListViewModel->moveRow({}, row, {}, row - 1);
}


void ExternalReadersCentralWidget::itemDown() {
    auto row = ui->listView->currentIndex().row();
    // TODO: it should be 1 instead of 2. check this with other version, maybe a Qt bug
    mListViewModel->moveRow({}, row, {}, row + 2);
}


// void ExternalReadersCentralWidget::browse() {
//     auto f = QFileDialog::getOpenFileName(this, tr("Markdown Editor"), QDir::homePath());
//     f      = f.simplified();
//     if (f.isEmpty()) return;
//     ui->editorLine->setText(f);
// }


void ExternalReadersCentralWidget::addItem() {
    auto item = ui->editorLine->text().simplified();
    if (item.isEmpty() || mListViewModel->stringList().contains(item)) {
        return;
    }

    int itemsCount = mListViewModel->rowCount();
    mListViewModel->insertRow(itemsCount);
    auto index = mListViewModel->index(itemsCount, 0);
    mListViewModel->setData(index, item);
    ui->editorLine->clear();
    ui->editorLine->setFocus();
}


void ExternalReadersCentralWidget::delItem() {
    int row = ui->listView->currentIndex().row();
    if (row < 0) {
        return;
    }

    mListViewModel->removeRow(row);
}


void ExternalReadersCentralWidget::accept() {
    Ui::Settings::saveExternalReaders(mListViewModel->stringList());
    emit accepted();
}
