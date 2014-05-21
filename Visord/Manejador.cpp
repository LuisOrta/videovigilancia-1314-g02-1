#include "Manejador.h"
int Manejador::sigHupFd[]={0,0};
int Manejador::sigTermFd[]={0,0};
int Manejador::sigIntFd[]={0,0};

Manejador::Manejador(QObject *parent) :
    QObject(parent)
{

   // Crear las parejas de sockets UNIX
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupFd))
        qFatal("No se pudo crear el par de socket para la señal HUP ");
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermFd))
        qFatal("No se pudo crear el par de socket para la señal TERM ");
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntFd))
        qFatal("No se pudo crear el par de socket para la señal INT ");

    // Crear los objetos para monitorizar uno de los socket
    // de cada pareja.

    sigHupNotifier = new QSocketNotifier(sigHupFd[1],
        QSocketNotifier::Read, this);
    sigTermNotifier = new QSocketNotifier(sigTermFd[1],
        QSocketNotifier::Read, this);
    sigIntNotifier = new QSocketNotifier(sigIntFd[1],
            QSocketNotifier::Read, this);

    // Conectar la señal activated() de cada objeto
    // QSocketNotifier con el slot correspondiente. Esta señal
    // será emitida cuando hayan datos para ser leidos en el
    // socket monitorizado.

    connect(sigHupNotifier, SIGNAL(activated(int)), this,
        SLOT(handleSigHup()));
    connect(sigTermNotifier, SIGNAL(activated(int)), this,
        SLOT(handleSigTerm()));
    connect(sigIntNotifier, SIGNAL(activated(int)), this,
            SLOT(handleSigInt()));

}

void Manejador::hupSignalHandler(int)
{

    char a = 1;
    ::write(sigHupFd[0], &a, sizeof(a));

}


void Manejador::termSignalHandler(int)
{

    char a = 1;
    ::write(sigTermFd[0], &a, sizeof(a));

}

void Manejador::intSignalHandler(int)
{

    char a = 1;
    ::write(sigIntFd[0], &a, sizeof(a));

}



int setupUnixSignalHandlers()
{

    struct ::sigaction hup, term ,interrup;

    hup.sa_handler = &Manejador::hupSignalHandler;
    ::sigemptyset(&hup.sa_mask);
    hup.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGHUP
    if (::sigaction(SIGHUP, &hup, 0) > 0)
    return 1;

    term.sa_handler = &Manejador::termSignalHandler;
    ::sigemptyset(&term.sa_mask);
    term.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGTERM
    if (::sigaction(SIGTERM, &term, 0) > 0)
    return 2;

    interrup.sa_handler = &Manejador::intSignalHandler;
    ::sigemptyset(&interrup.sa_mask);
    interrup.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGTERM
    if (::sigaction(SIGINT, &interrup, 0) > 0)
    return 3;


    return 0;


}
void Manejador::handleSigHup()
{

    sigHupNotifier->setEnabled(false);
    char tmp;
    ::read(sigHupFd[1], &tmp, sizeof(tmp));

    emit reiniciar();
    sigHupNotifier->setEnabled(true);

}

void Manejador::handleSigTerm()
{

    sigTermNotifier->setEnabled(false);
    char tmp;
    ::read(sigTermFd[1], &tmp, sizeof(tmp));
    emit terminar();
    sigTermNotifier->setEnabled(true);


}
void Manejador::handleSigInt()
{

    sigIntNotifier->setEnabled(false);
    char tmp;
    ::read(sigIntFd[1], &tmp, sizeof(tmp));
    emit terminar();
    sigIntNotifier->setEnabled(true);


}
