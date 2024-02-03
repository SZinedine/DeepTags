#include <QApplication>
#include <QTranslator>
#include <SingleApplication>
#include <cstring>
#include "MainWindow.h"

#ifndef DEEPTAGS_VERSION
    #error "The DEEPTAGS_VERSION flag isn't defined. Please define it and proceed"
#endif

int error();
const auto NAME             = QStringLiteral("DeepTagsRewrite");
const auto DEEPTAGS_WEBSITE = QStringLiteral("https://github.com/SZinedine/DeepTags");
const auto ORG_WEBSITE      = QStringLiteral("https://github.com/SZinedine");
const auto VERSION          = QStringLiteral(DEEPTAGS_VERSION);


int main(int argc, char* argv[]) {
    switch (argc) {
    case 1:
        break;
    case 2: {
        if (std::strcmp(argv[1], "-v") == 0 || std::strcmp(argv[1], "--version") == 0) {
            std::puts(QString("%1 %2").arg(NAME, VERSION).toStdString().c_str());
            return 0;
        } else if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
            const QString HELP_MESSAGE =
                QString("DeepTags %1 (%2)\n").arg(VERSION, DEEPTAGS_WEBSITE) +
                "Copyright (C) 2024 Zineddine SAIBI <saibi.zineddine@yahoo.com>.\n" +
                "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n" +
                "This is free software: you are free to change and redistribute it.\n";
            std::puts(HELP_MESSAGE.toStdString().c_str());

            return 0;
        } else {
            return error();
        }
    }
    default: {
        return error();
    }
    }

    SingleApplication app(argc, argv);
    QApplication::setApplicationName(NAME);
    QApplication::setOrganizationName(NAME);
    QApplication::setApplicationVersion(VERSION);
    QApplication::setOrganizationDomain(ORG_WEBSITE);

    auto locale = QLocale::system().name().section('_', 0, 0);
    // locale = "fr";      // for test
    auto translationFile = QString(":locale/DeepTags_%1.qm").arg(locale);
    QTranslator tr;
    tr.load(translationFile);
    QApplication::installTranslator(&tr);

    MainWindow win;
    win.setWindowTitle(NAME);
    win.setWindowIcon(QIcon((QStringLiteral(":images/icon128.png"))));
    QObject::connect(&app, &SingleApplication::instanceStarted, &win, &QMainWindow::raise);
    win.show();

    return QApplication::exec();
}


int error() {
    std::puts("Error. Invalid arguments");
    return 1;
}
