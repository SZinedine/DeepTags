#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include "gui/mainwindow.h"

#define VERSION  "0.4.2"
#define NAME     "DeepTags"
#define WEBSITE  "https://github.com/SZinedine"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(NAME);
    app.setApplicationVersion(VERSION);
    app.setOrganizationDomain(WEBSITE);
    app.setOrganizationName(NAME);

    QString     locale = ":locale/deeptags_" + QLocale::system().name().section('_', 0, 0) + ".qm";
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
