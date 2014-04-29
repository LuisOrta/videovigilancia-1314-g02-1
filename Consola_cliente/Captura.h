#ifndef HILO_CAPTURA_H
#define HILO_CAPTURA_H

#include <QThread>
#include <QCamera>
#include "captura_buffer.h"
#include "Manejador.h"
#include <QByteArray>
#include <QSettings>
#include <QPixmap>
#include <QBuffer>
#include <QImageWriter>
#include <QTcpSocket>

class Captura : public QObject
{
    Q_OBJECT


public:
    explicit Captura(const QList<QByteArray> &lista_camaras,QTcpSocket *socket =0,QObject *parent = 0);
    virtual ~Captura(void);

public slots:

    void empezar_capturar(void);


private slots:

    void showFrame(const QImage &rect);

 private:

    QCamera *cam_;
    Captura_Buffer *buffer_;
    QTcpSocket *socket_;

    QSettings conf_;
    QList<QByteArray>  dispositivos_;


};

#endif // HILO_CAPTURA_H
