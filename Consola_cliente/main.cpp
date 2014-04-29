#include <QCoreApplication>
#include <Consola.h>
//#include "Manejador.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Luis");
    QCoreApplication::setApplicationName("Visor");
    QCoreApplication a(argc, argv);
    Consola menu;



    return a.exec();
}
