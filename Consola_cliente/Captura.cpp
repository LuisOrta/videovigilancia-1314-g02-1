#include "Captura.h"



Captura::Captura(QObject *parent):
    QObject(parent),
    cam_(NULL),
    socket_(NULL),
    handle_(NULL),
    buffer_(NULL){
    socket_ = new QTcpSocket(this);
    qtout_ = new QTextStream(stdout,QIODevice::WriteOnly);
    buffer_ = new Captura_Buffer;
    handle_ = new Manejador(this);

    signal(SIGINT,Manejador::intSignalHandler);
    signal(SIGHUP,Manejador::hupSignalHandler);
    signal(SIGTERM,Manejador::termSignalHandler);

    dispositivos_ =  QCamera::availableDevices();
    connect(handle_,SIGNAL(terminar()),this,SLOT(terminar_captura()));
    connect(socket_,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(buffer_, SIGNAL(mostrar_marco(QImage)), this, SLOT(showFrame(const QImage&)));
}
Captura::~Captura(){

   delete socket_;
   delete  cam_;
   delete  buffer_;
   delete  qtout_;
   delete handle_;

   handle_ = NULL;
   qtout_ = NULL;
   cam_ = NULL;
   buffer_ = NULL;
   socket_ = NULL;
}
void Captura::displayError(QAbstractSocket::SocketError socketError)
{
    *qtout_ << "------------------------------------------" << endl;
    *qtout_ << "- ADVERTENCIA - SE HA PRODUCIDO UN ERROR -" << endl;
    *qtout_ << "------------------------------------------" << endl;

   switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
       {
         *qtout_ << "Se ha cerrado la conexion con el host remoto" << endl;
            if(cam_ != NULL && cam_->ActiveStatus == 8)
            {
                    cam_->stop();
                    socket_->abort();
            }
            break;
       }
    case QAbstractSocket::HostNotFoundError:
        *qtout_ << "El host no se ha encontrado. Por favor, revise la dirección del host y el puerto en la configuración." << endl;
         emit mostrar_menu_general();
         break;

    case QAbstractSocket::ConnectionRefusedError:
        *qtout_ << " No se ha podido establecer la conexion " << endl;
         emit mostrar_menu_general();
        break;
    default:
        *qtout_ << " Ha ocurrido el siguiente error " << socket_->errorString() <<endl;
         emit mostrar_menu_general();
         if(cam_ != NULL && cam_->ActiveState == 8)
         {
             cam_->stop();
         }
    }
   *qtout_ << "\n";

}

void Captura::empezar_captura (const QString &ip,const QString &puerto,const QString &dispositivo_sel)
{

        this->socket_->connectToHost(ip,puerto.toUInt());
        if(socket_->waitForConnected()==true)
        {
            if(socket_->state() == QTcpSocket::ConnectedState)
            {
                int i = 0;
                bool encontrado = false;
                for( ; i<dispositivos_.length() && encontrado == false;i++){

                    if(dispositivos_[i] == dispositivo_sel){
                       encontrado = true;
                    }

                }
                i = i-1;
                if (encontrado == true)
                {

                       if(cam_ != NULL && cam_->ActiveStatus == 8)
                       {
                            cam_->stop();
                            delete cam_;
                           cam_ = NULL;
                        }
                        cam_ = new QCamera(dispositivos_[i]);
                        if(cam_ != NULL)
                        {
                            cam_->setViewfinder(buffer_);
                            cam_->setCaptureMode(QCamera::CaptureViewfinder);
                            cam_->start();
                        }
                       else
                        {
                               *qtout_ << " ERROR: No se ha podido asignar memoria a la camara " << endl;
                               mostrar_menu_general();
                        }


                }
            }
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
        if (socket_->isValid())
        {
            socket_->write(reinterpret_cast<char*>(&size), 4);
            socket_->write(reinterpret_cast<char*>(&version), 4);
            socket_->write(buffer.data());
         }

 }
void  Captura::terminar_captura(void)
{

        if(socket_ != NULL)
        {
            if(socket_->state() == QTcpSocket::ConnectedState || socket_->state() == QTcpSocket::ConnectingState)
             {
                if(cam_->ActiveStatus == 8)
                {
                    cam_->stop();
                    socket_->abort();

                        emit captura_terminada();

                }

            }
            emit mostrar_menu_general();

        }
}
