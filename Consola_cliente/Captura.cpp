#include "Captura.h"



Captura::Captura(const QList<QByteArray> &lista_camaras,QTcpSocket *socket,QObject *parent):
    QObject(parent),
    cam_(NULL),
    socket_(socket),
    dispositivos_(lista_camaras),
    buffer_(NULL)
{
    int i = 0;
    bool encontrado = false;
    for( ; i<lista_camaras.length() && encontrado == false;i++){

        if(lista_camaras[i] == conf_.value("Camera", "/dev/video0")){
           encontrado = true;
        }

    }
    i = i-1;
    if (encontrado == true)
    {
       cam_ = new QCamera(lista_camaras[i]);
       buffer_ = new Captura_Buffer;
       cam_->setViewfinder(buffer_);
       cam_->setCaptureMode(QCamera::CaptureViewfinder);
       connect(buffer_, SIGNAL(mostrar_marco(QImage)), this, SLOT(showFrame(const QImage&)));
    }

}
 Captura::~Captura(){


    if (cam_ != NULL)
    {
        if(cam_->state() == QCamera::ActiveState)
        {
            cam_->stop();
        }
    }


    delete  cam_;
    delete  buffer_;

    cam_ = NULL;
    buffer_ = NULL;
}

void Captura::empezar_capturar(void)
{
    if (this->socket_->state() == 3)
    {
        cam_->start();
    }
}


void Captura::showFrame(const QImage& rect)
 {
    // Preguntar a Jesús : creo que falla porque al ser una aplicación de consola no se puede implementar este tipos de objetos que utilizan la pantalla
       //QPixmap frame;
       //frame.convertFromImage(rect,Qt::AutoColor);

       //Envio de la imagen por socket
       QBuffer buffer;
       int version = 1;
       QImageWriter writer(&buffer, "jpeg");
       writer.setCompression(70);
       writer.write(rect);
       int size = buffer.data().size();
       socket_->write(reinterpret_cast<char*>(&size), 4);
       socket_->write(reinterpret_cast<char*>(&version), 4);
       socket_->write(buffer.data());

 }

