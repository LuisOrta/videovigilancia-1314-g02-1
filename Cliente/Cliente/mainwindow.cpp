#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"
#include "QFileDialog"
#include "QMessageBox"
#include "QImageReader"
#include "QMutex"
#include "QWaitCondition"
#include "QMovie"
#include "QSettings"
#include "acercade.h"
#include "QCamera"
#include "QCameraViewfinder"
#include "capturebuffer.h"
#include "selcamara.h"
#include "QBuffer"
#include "QImageWriter"
#include "selport.h"
#include "opencv2/opencv.hpp"
#include "cvmatandqimage.h"
#include <QString>


using namespace QtOcv;

#define VERSION 1



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    buffer(NULL),
    socket(NULL)
{

    QSettings sett("ficheroCliente.ini", QSettings::IniFormat);
    ui->setupUi(this);
    ui->checkBox->setChecked(sett.value("Autorun", true).toBool());

    devices = QCamera::availableDevices();

    cam = new QCamera();
    isInicio = true;

    socket = new QTcpSocket(this);
    buffer = new capturebuffer;
    cierre = 0;

    //cv::BGSubstractor.set("nmixtures", 3);
    if(socket != NULL)
    {
     connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(manipuladorError(QAbstractSocket::SocketError)));
     connect(socket,SIGNAL(disconnected()),this,SLOT(socketDesconectado()));
    }
    if(buffer != NULL)
    {
     connect(buffer, SIGNAL(mostrar(const QImage&)), this, SLOT(showFrame(const QImage&)));
    }
 }


MainWindow::~MainWindow()
{
    delete ui;
    delete cam;
    delete buffer;
    delete socket;
}

void MainWindow::on_actionAbrir_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this, "Abrir archivo", QString(), "Video (*.mjpeg)");

    movie.setFileName(filename);
    ui->label->setMovie(&movie);
    //connect(&movie, SIGNAL(updated(const QRect&)), this, SLOT(showFrame(const QRect&)));

    if (ui->checkBox->isChecked()){
        movie.start();
    }
    connect(ui->Play, SIGNAL(clicked()), &movie, SLOT(start()));
    connect(ui->Stop, SIGNAL(clicked()), &movie, SLOT(stop()));

}



void MainWindow::on_checkBox_clicked()
{
    QSettings sett("ficheroCliente.ini", QSettings::IniFormat);
    sett.setValue("Autorun", ui->checkBox->isChecked());
}



void MainWindow::on_actionAcerca_de_triggered()
{
    AcercaDe ad(this);
    ad.exec();
}


QImage MainWindow::detect_mov(cv::Mat imagen)
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
    imagen_dev=mat2Image(imagen);
    return (imagen_dev);
}



void MainWindow::showFrame(const QImage& rect)
{
    int protocol_version = 1;

    QImage imagen_dm;
    cv::Mat mat_img;
    mat_img = image2Mat(rect);
    imagen_dm = detect_mov(mat_img);

    QPixmap frame;
    frame.convertFromImage(imagen_dm,Qt::AutoColor);
    ui->label->setPixmap(frame);

    //Envio de la imagen por socket
    QBuffer buffer;
    QImageWriter writer(&buffer, "jpeg");
    writer.setCompression(70);
    writer.write(imagen_dm);
    qDebug() << QByteArray::number(buffer.data().size());
    int size = buffer.data().size();
    socket->read(reinterpret_cast<char*>(&cierre), 4);
    if (cierre == 202){
        qDebug() << "DESCONECTADO";
        socket->disconnectFromHost();
    }
    else{
        if(enviar){
            qDebug() << "ENVIANDO";
            int tam_vrec = r.size();

            socket->write(reinterpret_cast<char*>(&size), 4);
            socket->write(reinterpret_cast<char*>(&protocol_version), 4);
            socket->write(reinterpret_cast<char*>(&n_camara), 4);
            socket->write(reinterpret_cast<char*>(&tam_vrec), 4);

            while(!r.empty()){
                int x = r.back().x;
                int y = r.back().y;
                int w = r.back().width;
                int h = r.back().height;

                r.pop_back();

                socket->write(reinterpret_cast<char*>(&x), 4);
                socket->write(reinterpret_cast<char*>(&y), 4);
                socket->write(reinterpret_cast<char*>(&w), 4);
                socket->write(reinterpret_cast<char*>(&h), 4);
                qDebug() << x;
                qDebug() << y;
                qDebug() << w;
                qDebug() << h;
            }

            socket->write(buffer.data());
        }
        else qDebug() << "NO ENVIADO";
    }
}





void MainWindow::on_actionCapturar_desde_WebCam_triggered()
{
    if(socket->state() != QTcpSocket::ConnectedState &&  socket->state() != QTcpSocket::ConnectingState)
    {
            QSettings sett("ficheroCliente.ini", QSettings::IniFormat);
            qDebug() << sett.value("IPServer", "127.0.0.1").toString();
            socket->connectToHost(sett.value("IPServer", "127.0.0.1").toString(),sett.value("IPPort", 15000).toInt());


            if (socket->waitForConnected()==true){

                if((cam->state() != QCamera::ActiveState && cam->status() != 8 )|| isInicio){
                    //Crear la camara y el visor
                    for(int i=0; i<devices.length();i++){
                        if(devices[i]== sett.value("Camera", "/dev/video0")){
                            cam = new QCamera(devices[i]);
                            n_camara = i + 1;
                            isInicio = false;
                        }
                    }


                    //Establecer buffer como visor de la cam
                    cam->setViewfinder(buffer);
                    cam->setCaptureMode(QCamera::CaptureViewfinder);
                    cam->start();
                    //Conectar la señal mostrar de present() con el slot de ShowFrame()

                }

                else{
                    cam->stop();
                }

            }
       }
}



void MainWindow::on_actionSeleccionar_Camara_triggered()
{
    SelCamara sel;
    sel.exec();
}

void MainWindow::on_actionCambiar_IP_Puerto_triggered()
{
    selport sel;
    sel.exec();
}

void MainWindow::on_actionParar_Captura_triggered()
{
    int cerrar = 1;
    cam->stop();
    socket->write(reinterpret_cast<char*>(&cerrar), 4);
    socket->abort();
}

void MainWindow::manipuladorError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Visor Cliente"),
                                 tr("El host no se ha podido encontrar, Por favor revise"
                                    "el nombre del host que ha usado para establecer la conexión"));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Visor Cliente"),
                                 tr("La conexión ha sido rechazada en el extremo , por favor revise "
                                    " la configuración establecida "));
        break;
    default:
        QMessageBox::information(this, tr("Visor Cliente"),
                                 tr("Ha ocurrido el siguiente error : %1.")
                                 .arg(socket->errorString()));
    }
    if(cam != NULL && cam->ActiveStatus == 8)
    {
        cam->stop();
    }
    if(socket != NULL && (socket->state() == QTcpSocket::ConnectedState || socket->state() == QTcpSocket::ConnectingState))
    {
        socket->abort();
    }
}
void MainWindow::socketDesconectado()
{

    if(cam != NULL && cam->ActiveStatus == 8)
    {
        cam->stop();
    }
    if(socket != NULL && (socket->state() == QTcpSocket::ConnectedState || socket->state() == QTcpSocket::ConnectingState))
    {
        socket->abort();
    }

}
