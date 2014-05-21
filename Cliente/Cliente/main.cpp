#include "mainwindow.h"
#include <QApplication>
#include "QSettings"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Luis");
    QCoreApplication::setApplicationName("Visor");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
