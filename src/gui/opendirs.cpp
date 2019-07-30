#include "opendirs.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QSettings>
#include <QRegExp>

OpenDirs::OpenDirs(QDialog *parent)
    : QDialog(parent)
{
    construct();
    setFixedSize(400, 250);
    setModal(true);
    load();
}

void OpenDirs::construct() {
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QLabel* label = new QLabel(tr("Put the directories to open on startup\nOne directory in every line:"));
    textWidget = new QTextEdit(this);
    textWidget->setAcceptRichText(false);
    okButton = new QPushButton(tr("Ok"));
    cancelButton = new QPushButton(tr("Cancel"));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
    layout->addWidget(textWidget);
    layout->addLayout(buttonsLayout);

    connect(okButton, 		&QPushButton::clicked, 	this, 	&OpenDirs::accept);
    connect(cancelButton, 	&QPushButton::clicked, 	this, 	&OpenDirs::reject);
    connect(this, 			&OpenDirs::accepted,	this,	&OpenDirs::save);
}


void OpenDirs::save() {
    QString txt = textWidget->toPlainText();
    QSettings settings;
    settings.beginGroup("Main");
    settings.setValue("open_dirs", QVariant(txt));
    settings.endGroup();
}

void OpenDirs::load() {

    textWidget->setText(savedText());
}

QString OpenDirs::savedText() {
    QSettings settings;
    settings.beginGroup("Main");
    QString str = settings.value("open_dirs").toString();
    settings.endGroup();
    return str;
}

QStringList OpenDirs::savedTextList() {
    QString str = OpenDirs::savedText();
    return str.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
}
