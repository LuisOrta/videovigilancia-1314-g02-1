#ifndef HILO_CAPTURA_H
#define HILO_CAPTURA_H

#include <QThread>
#include <QCamera>
#include "captura_buffer.h"
#include "Manejador.h"
#include <QByteArray>
#include <QSettings>
#include <QBuffer>
#include <QImageWriter>
#include <QTcpSocket>
#include <QTextStream>
#include <QHostAddress>
#include <Manejador.h>
#include "opencv2/opencv.hpp"
#include "cvmatandqimage.h"


class Captura : public QObject
{
    Q_OBJECT


public:

    explicit Captura(QObject *parent = 0);
    virtual ~Captura(void);
    bool enviar;
    typedef std::vector<cv::Rect> vector_rectangulos;
    vector_rectangulos r;
    cv::BackgroundSubtractorMOG2 BGSubstractor;
    int cierre;


signals:

    void mostrar_menu_general(void);
    void captura_terminada(void);

public slots:

    void displayError(QAbstractSocket::SocketError socketError);
    void empezar_captura (const QString &ip,const QString &puerto,const QString &dispositivo_sel = NULL);
    void terminar_captura(void);

private slots:

    void showFrame(const QImage &rect);
    QImage detect_mov(cv::Mat imagen);


 private:

    QCamera *cam_;
    Captura_Buffer *buffer_;
    QTcpSocket *socket_;
    QTextStream *qtout_;
    Manejador *handle_;
    QSettings conf_;
    QList<QByteArray>  dispositivos_;
    int n_camara_;


};

#endif // HILO_CAPTURA_H
