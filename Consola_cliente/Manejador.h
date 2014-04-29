#ifndef MANEJADOR_H
#define MANEJADOR_H

#include <QObject>
#include <QSocketNotifier>
#include <signal.h>
#include <sys/socket.h>
#include <QTextStream>

class Manejador : public QObject
{
    Q_OBJECT

public:
    Manejador(QObject *parent = 0);

    // Manejadores de señal POSIX
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
    static void intSignalHandler(int unused);

public slots:

    // Slots Qt donde atender las señales POSIX
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();

signals:

    void terminar(void);

private:

    // Pares de sockets. Un par por señal a manejar
    static int sigHupFd[2];
    static int sigTermFd[2];
    static int sigIntFd[2];

    // Objetos para monitorizar los pares de sockets
    QSocketNotifier *sigHupNotifier;
    QSocketNotifier *sigTermNotifier;
    QSocketNotifier *sigIntNotifier;
};

#endif // MANEJADOR_H
