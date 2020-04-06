#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <cstring>
#include "gui/mainwindow.h"

#define NAME    "DeepTags"
#define DEEPTAGS_WEBSITE "https://github.com/SZinedine/DeepTags"
#define ORG_WEBSITE "https://github.com/SZinedine"

#ifndef __cpp_lib_filesystem
    #error \
        "File system library isn't supported. Please use a compiler that supports the 2017 standard of C++"
#endif

#ifdef INCLUDE_SINGLE_APPLICATION
    #include <SingleApplication>
    #define _QAPP SingleApplication
#else
    #define _QAPP QApplication
#endif

#ifndef INCLUDE_SINGLE_APPLICATION
    #warning "SingleApplication isn't included."
#elif !defined(INCLUDE_QBREEZE)
    #warning "QBreeze isn't included"
#elif !defined(INSIDE_EDITOR)
    #warning "QMarkdownTextEdit isn't included"
#endif

#ifndef DEEPTAGS_VERSION
    #error "The DEEPTAGS_VERSION flag isn't defined. Please define it and proceed"
#endif

#define HELP_MESSAGE                                                                   \
    "DeepTags " DEEPTAGS_VERSION " (" DEEPTAGS_WEBSITE ")\n"                                    \
    "Copyright (C) 2020 Zineddine SAIBI <saibi.zineddine@yahoo.com>.\n"                \
    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n" \
    "This is free software: you are free to change and redistribute it.\n"


int main(int argc, char* argv[]) {
    if (argc == 2) {
        if (std::strcmp(argv[1], "-v") == 0 || std::strcmp(argv[1], "--version") == 0) {
            std::cout << DEEPTAGS_VERSION << std::endl;
            return 0;
        } else if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
            std::cout << HELP_MESSAGE << std::endl;
            return 0;
        } else {
            std::cout << "ERROR. Try to run DeepTags without any argument." << std::endl;
            return 1;
        }
    }
    _QAPP app(argc, argv);
    app.setApplicationName(NAME);
    app.setApplicationVersion(DEEPTAGS_VERSION);
    app.setOrganizationDomain(ORG_WEBSITE);
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
