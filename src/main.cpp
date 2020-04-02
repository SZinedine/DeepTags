#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "gui/mainwindow.h"

#define VERSION "0.5"
#define NAME    "DeepTags"
#define WEBSITE "https://github.com/SZinedine"

#ifdef INCLUDE_SINGLE_APPLICATION
#include <SingleApplication>
#define _QAPP SingleApplication
#else
#define _QAPP QApplication
#endif

int main(int argc, char* argv[]) {
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
