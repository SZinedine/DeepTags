#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "gui/mainwindow.h"
#include <cstring>

#define VERSION "0.5"
#define NAME    "DeepTags"
#define WEBSITE "https://github.com/SZinedine"

#ifdef INCLUDE_SINGLE_APPLICATION
#include <SingleApplication>
#define _QAPP SingleApplication
#else
#define _QAPP QApplication
#endif

#define HELP_MESSAGE \
    "DeepTags " VERSION "\n" \
    "Copyright (C) 2020 Zineddine SAIBI <saibi.zineddine@yahoo.com>.\n"\
    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"\
    "This is free software: you are free to change and redistribute it."

int main(int argc, char* argv[]) {
    if (argc == 2) {
        if (std::strcmp(argv[1], "-v") == 0 || std::strcmp(argv[1], "--version") == 0) {
            std::cout << VERSION << std::endl;
            return 0;
        }
        else if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
            std::cout << HELP_MESSAGE << std::endl;
            return 0;
        }
        else {
            std::cout << "ERROR. Try to run DeepTags without any argument." << std::endl;
            return 1;
        }
    }
    _QAPP app(argc, argv);
    app.setApplicationName(NAME);
    app.setApplicationVersion(VERSION);
    app.setOrganizationDomain(WEBSITE);
    app.setOrganizationName(NAME);

    QString locale = ":locale/deeptags_" + QLocale::system().name().section('_', 0, 0) + ".qm";
    QTranslator tran;
    // tran.load(":locale/deeptags_fr.qm");		// for test
    tran.load(locale);
    app.installTranslator(&tran);

    MainWindow w;
    w.setWindowTitle(QString(NAME));
    w.setWindowIcon(QIcon(":images/icon128.png"));
    w.show();

    return app.exec();
}
