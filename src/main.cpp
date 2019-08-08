#include "gui/mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#define VERSION "0.3.3"
#define TITLEBAR "DeepTags v."
#define NAME "DeepTags"
#define WEBSITE "https://github.com/SZinedine"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(NAME);
    app.setApplicationDisplayName(NAME);
    app.setApplicationVersion(VERSION);
    app.setOrganizationDomain(WEBSITE);
    app.setOrganizationName(NAME);

//    QString locale = QLocale::system().name().section('_', 0, 0);
//    QTranslator tran;
////    tran.load("deeptags_fr");		// for test
//    tran.load("deeptags_" + locale);
//    app.installTranslator(&tran);

    MainWindow w;
    w.setWindowTitle(QString(TITLEBAR) + QString(VERSION));
    w.setWindowIcon(QIcon(":images/icon128.png"));
    w.show();

    return app.exec();
}
