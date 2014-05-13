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
#include "selport.h"
#include "QtEndian"

#define VERSION 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(NULL),
    clientConnection(NULL)
{
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    ui->setupUi(this);
    ui->checkBox->setChecked(sett.value("Autorun", true).toBool());

    devices = QCamera::availableDevices();
    cam = new QCamera();
    isInicio = true;

    server = new QTcpServer(this);
    server->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
    connect(server, SIGNAL(newConnection()), this, SLOT(escucha()));
    clientConnection = new QTcpSocket(this);
    buffsize = 0;
    corrupt_counter = 0;
    cont_rect = 0;
    protocol_version = 0;
    n_rec = 0;
    terminado_roi = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cam;
    delete buffer;
    delete server;
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
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    sett.setValue("Autorun", ui->checkBox->isChecked());
}



void MainWindow::on_actionAcerca_de_triggered()
{
    AcercaDe ad(this);
    ad.exec();
}

void MainWindow::showFrame(const QImage& rect)
{

    qDebug() << "MOSTRANDO!";
   /* QPixmap frame;
    frame.convertFromImage(rect,Qt::AutoColor);
    ui->label->setPixmap(frame);*/

    //REINICIAR READBUFFER y BUFFSIZE

}





void MainWindow::escucha()
{
    while(server->hasPendingConnections()){
            qDebug() << "CONECTADO!";
            clientConnection = server->nextPendingConnection();
            corrupt_counter = 0;
            connect(clientConnection, SIGNAL(readyRead()), this, SLOT(leerDatos()));
    }

}


void MainWindow::leerDatos()
{
    u_int32_t cerrar = 202;


    qDebug() << "LEYENDO DATOS!";
    while(clientConnection->bytesAvailable()>=4){
        if (buffsize == 0){
            if (clientConnection->bytesAvailable()>=4){//TAMAÑO
                clientConnection->read(reinterpret_cast<char*>(&buffsize), sizeof(buffsize));
                buffsize = qToLittleEndian(buffsize);
                qDebug() << "LEYENDO TAMAÑO!";
                qDebug() << buffsize;
            }
            if (buffsize > 50000 || buffsize == 1){
                clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                clientConnection->disconnectFromHost();
            }
         }

        while (protocol_version == 0){
            if (clientConnection->bytesAvailable()>=4 && protocol_version == 0){ //VERSION
                clientConnection->read(reinterpret_cast<char*>(&protocol_version), sizeof(protocol_version));
                protocol_version = qToLittleEndian(protocol_version);
                qDebug() << "VERSION!";
                qDebug() << protocol_version;
                if (protocol_version != VERSION){
                    clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                    clientConnection->disconnectFromHost();
                }
            }
          }


        while (n_rec == 0){
            if (clientConnection->bytesAvailable()>=4 && n_rec == 0){ //NUMERO DE RECTANGULOS
                clientConnection->read(reinterpret_cast<char*>(&n_rec), sizeof(n_rec));
                n_rec= qToLittleEndian(n_rec);
                qDebug() << "N de RECTANGULOS";
                qDebug() << n_rec;
            }
        }


        while(cont_rect < n_rec && terminado_roi == false){
            qDebug() << "PETADA!";
            if (clientConnection->bytesAvailable()>=16){
                rect aux_rect;
                clientConnection->read(reinterpret_cast<char*>(&aux_rect.x), sizeof(aux_rect.x));
                aux_rect.x= qToLittleEndian(aux_rect.x);
                qDebug() << aux_rect.x;

                clientConnection->read(reinterpret_cast<char*>(&aux_rect.y), sizeof(aux_rect.y));
                aux_rect.y= qToLittleEndian(aux_rect.y);
                qDebug() << aux_rect.y;

                clientConnection->read(reinterpret_cast<char*>(&aux_rect.w), sizeof(aux_rect.w));
                aux_rect.w= qToLittleEndian(aux_rect.w);
                qDebug() << aux_rect.w;

                clientConnection->read(reinterpret_cast<char*>(&aux_rect.h), sizeof(aux_rect.h));
                aux_rect.h= qToLittleEndian(aux_rect.h);
                qDebug() << aux_rect.h;

                v_rect.push_back(aux_rect);

                cont_rect++;
            }

        }
        cont_rect = 0;
        terminado_roi = true;

        while(readbuffer.length() < buffsize && clientConnection->bytesAvailable()>0){
            if (readbuffer.length() < buffsize){
                 readbuffer.append(clientConnection->read(buffsize - readbuffer.length()));
                 qDebug() << "TAMAÑO BUFFER!";
                 qDebug() << readbuffer.length();
            }

            if(readbuffer.length() == buffsize){
                 QFile file("imagenCargada.jpg");
                 file.open(QIODevice::WriteOnly);
                 file.write(readbuffer, buffsize);
                 bool carga = imagebuff.loadFromData(readbuffer.data(), "jpeg");

                 /*-------------------------------------------------------*/
                 /*FUNCION DE ALMACENAR METADATOS (STD::VECTOR)*/
                 /*VACIAR EL VECTOR EN LA FUNCION CUANDO SE TENGA*/
                 while(!v_rect.empty()){
                    v_rect.pop_back();
                 }
                 /*-------------------------------------------------------*/
                 //showFrame(imagebuff);
                 readbuffer.remove(0, buffsize);
                 buffsize = 0;
                 protocol_version = 0;
                 n_rec = 0;
                 terminado_roi = false;

            }
        }
    }
}




void MainWindow::on_actionSeleccionar_Camara_triggered()
{
    SelCamara sel;
    sel.exec();
}

void MainWindow::on_actionSeleccionar_puerto_triggered()
{
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    selport sel;
    sel.exec();
    server->close();
    bool x = server->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
    qDebug() << x;
}
