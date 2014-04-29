#ifndef CONSOLA_H
#define CONSOLA_H

#include <QObject>
#include <iostream>
#include <QTextStream>
#include <QSettings>
#include <QCamera>
#include <QByteArray>
#include <QTcpSocket>
#include <QWaitCondition>
#include <QDebug>
#include "Captura.h"
#include "captura_buffer.h"
#include "Manejador.h"
#include <QThread>

class Consola : public QObject
{
    Q_OBJECT
public:
    explicit Consola(QObject *parent = 0);
    ~Consola();

    void mostrar_menu(void);

private slots:

    void displayError(QAbstractSocket::SocketError socketError);
    void cerrar_objetos(void);


private:

    QTextStream *qtin_;
    QTextStream *qtout_;
    QTcpSocket *socket_;
    QThread *hilo_captura_;
    Captura *objeto_captura_;


    QString eleccion_;
    QString dispositivo_sel_;
    QString ip_;
    QString puerto_;

    Manejador *handle_;
    QSettings conf_;
    QList<QByteArray> dispositivos_;




    void mostrar_menu_capturar(void);
    void mostrar_menu_configuracion(void);
    void mostrar_menu_configuracion_ip(void);
    void mostrar_menu_configuracion_puerto(void);
    void mostrar_menu_configuracion_dispositivo_video(void);
};
#endif // CONSOLA_H
