#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QSettings>
#include <QtEndian>
#include <QDebug>
#include <QFile>
#include "Manejador.h"
#include "syslog.h"
#include "QtSql/QSqlDatabase"
#include "QtSql/qsqlquery.h"
#include "QDateTime"
#include "QImage"
#include <syslog.h>
#include <stdio.h>
#include <QHash>
#define FICHERO_PID "/var/run/Visord.pid"
#define VERSION 1


class Servidor : public QObject
{
    Q_OBJECT
public:
    explicit Servidor(QObject *parent = 0);
    ~Servidor();


private:

    struct rect{
      int x;
      int y;
      int w;
      int h;
    };
    struct flags_protocols{

        u_int32_t buffsize_;
        u_int32_t protocol_version_;
        u_int32_t n_rec_;
        int n_camara_;
        QByteArray readbuffer_;
        int cont_rect_;
        std::vector<rect> v_rect_;
        bool terminado_roi_;
    };
    QHash <QTcpSocket *,flags_protocols> hash_clientsconnections;
    QTcpServer *server_;
    Manejador *handle_;

    QList <QTcpSocket *> clientConnections_;

    QImage imagebuff_;
    int corrupt_counter_;
    int cont_rect_;

    QSqlDatabase db_;
    std::vector<rect> v_rect_;

private slots:


    void escucha();
    void reiniciar();
    void cerrar();
    void clienteDesconectado();
    void leerDatos();
    void almacenar_metadatos(int nombre, std::vector<rect> metadatos);

};

#endif // SERVIDOR_H
