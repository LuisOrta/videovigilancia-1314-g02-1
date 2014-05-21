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
    QTcpServer *server_;
    Manejador *handle_;

    QList <QTcpSocket *> clientConnections_;
    QByteArray readbuffer_;

    u_int32_t buffsize_;
    u_int32_t protocol_version_;
    u_int32_t n_rec_;
    bool terminado_roi_;
    int corrupt_counter_;
    int cont_rect_;
    int n_camara_;
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
