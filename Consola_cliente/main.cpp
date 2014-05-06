#include <QCoreApplication>
#include <Consola.h>
#include <Captura.h>
#include <qthread.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Luis");
    QCoreApplication::setApplicationName("Visor");
    QCoreApplication a(argc, argv);
    QThread hilo_menu;
    Captura Capturador;
    Consola menu(&Capturador);


    menu.moveToThread(&hilo_menu);
    hilo_menu.start();
    menu.mostrar_menu();

    return a.exec();
    hilo_menu.quit();
    hilo_menu.wait();
}
