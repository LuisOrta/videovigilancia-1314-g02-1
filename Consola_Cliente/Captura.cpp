#include "Captura.h"
#include "opencv2/opencv.hpp"
#include "cvmatandqimage.h"

using namespace QtOcv;

Captura::Captura(QObject *parent):
    QObject(parent),
    cam_(NULL),
    socket_(NULL),
    conf_("ficheroConsolaCliente.ini", QSettings::IniFormat),
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
QImage Captura::detect_mov(cv::Mat imagen)
{
    //DETECTAR MOVIMIENTO
    enviar = false;
    typedef std::vector<std::vector<cv::Point> > ContoursType;


    cv::Mat foregroundMask;
    BGSubstractor(imagen, foregroundMask);


    for (int i=0;i<10;i++)
        cv::erode(foregroundMask, foregroundMask, cv::Mat());

    for (int i=0;i<10;i++)
        cv::dilate(foregroundMask, foregroundMask, cv::Mat());


    ContoursType contours;
    cv::findContours(foregroundMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);



    //PINTAR CUADROS
    if(!contours.empty()){
        for(int i= 0; i< contours.size(); i++){
            r.push_back(cv::boundingRect(contours[i]));
            cv::rectangle(imagen, r[i], cvScalar(0, 255, 0, 0));
            enviar = true;
        }
    }

    //CONVERTIR A QIMAGE PARA DEVOLVER
    QImage imagen_dev;
    imagen_dev = mat2Image(imagen);
    return (imagen_dev);
}


void Captura::showFrame(const QImage& rect)
 {
    int protocol_version = 1;

    QImage imagen_dm;
    cv::Mat mat_img;
    mat_img = image2Mat(rect);
    imagen_dm = detect_mov(mat_img);


    //Envio de la imagen por socket
    QBuffer buffer;
    QImageWriter writer(&buffer, "jpeg");
    writer.setCompression(70);
    writer.write(imagen_dm);
    qDebug() << QByteArray::number(buffer.data().size());
    int size = buffer.data().size();
    socket_->read(reinterpret_cast<char*>(&cierre), 4);
    if (cierre == 202){
        qDebug() << "DESCONECTADO";
        socket_->disconnectFromHost();
    }
    else{
        if(enviar){
            qDebug() << "ENVIANDO";
            int tam_vrec = r.size();

            socket_->write(reinterpret_cast<char*>(&size), 4);
            socket_->write(reinterpret_cast<char*>(&protocol_version), 4);
            socket_->write(reinterpret_cast<char*>(&tam_vrec), 4);
            while(!r.empty()){
                int x = r.back().x;
                int y = r.back().y;
                int w = r.back().width;
                int h = r.back().height;

                r.pop_back();

                socket_->write(reinterpret_cast<char*>(&x), 4);
                socket_->write(reinterpret_cast<char*>(&y), 4);
                socket_->write(reinterpret_cast<char*>(&w), 4);
                socket_->write(reinterpret_cast<char*>(&h), 4);
                qDebug() << x;
                qDebug() << y;
                qDebug() << w;
                qDebug() << h;
            }

            socket_->write(buffer.data());
        }
        else qDebug() << "NO ENVIADO";
    }

 }
void  Captura::terminar_captura(void)
{

        if(socket_ != NULL)
        {
            if(socket_->state() == QTcpSocket::ConnectedState)
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
