#include "gui/mainwindow.h"
#include <QApplication>

#define VERSION "0.1.2"
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
    app.setOrganizationName("SZ");

    MainWindow w;
    w.setWindowTitle(QString(TITLEBAR) + QString(VERSION));
    w.setWindowIcon(QIcon(":images/icon128.png"));
    w.show();

    return app.exec();
}
