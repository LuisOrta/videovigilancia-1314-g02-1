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

#define VERSION 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(NULL),
    clientConnection(NULL)
{
    QSettings sett;
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
    QSettings sett;
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
    QPixmap frame;
    frame.convertFromImage(rect,Qt::AutoColor);
    ui->label->setPixmap(frame);

    //REINICIAR READBUFFER y BUFFSIZE
    readbuffer.remove(0, buffsize);
    buffsize = 0;

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
    int cerrar = 202;

    int corrupt = 203;

    qDebug() << "LEYENDO DATOS!";
    while(buffsize == 0 && clientConnection->bytesAvailable()>=4){
        if (buffsize == 0){
            if (clientConnection->bytesAvailable()>=4){
                clientConnection->read(reinterpret_cast<char*>(&buffsize), 4);
                qDebug() << "LEYENDO TAMAÑO!";
                qDebug() << buffsize;
            }
            if (buffsize > 35000 || buffsize == 1){
                clientConnection->write(reinterpret_cast<char*>(&cerrar), 4);
                clientConnection->disconnectFromHost();
            }
         }

        if (clientConnection->bytesAvailable()>=4){
            clientConnection->read(reinterpret_cast<char*>(&protocol_version), 4);
            qDebug() << "VERSION!";
            qDebug() << protocol_version;
            if (protocol_version != VERSION){
                clientConnection->write(reinterpret_cast<char*>(&cerrar), 4);
                clientConnection->disconnectFromHost();
            }

        }



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

                 //DESCOMENTAR CUANDO ARREGLE LO DE LAS IMAGENES CORRUPTAS
                 /*if (carga){
                     if (corrupt_counter > 0)
                        corrupt_counter--;
                 }
                 else{
                     corrupt_counter ++;
                 }

                 if (corrupt_counter >=20){
<<<<<<< HEAD
                     clientConnection->write(reinterpret_cast<char*>(&corrupt), 4);
=======
                     clientConnection->write(reinterpret_cast<char*>(&cerrar), 4);
>>>>>>> bdca955b59030d2ba5bc466a529879f29aa4b096
                     clientConnection->disconnectFromHost();
                 }*/

                 showFrame(imagebuff);
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
    QSettings sett;
    selport sel;
    sel.exec();
    server->close();
    bool x = server->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
    qDebug() << x;
}
