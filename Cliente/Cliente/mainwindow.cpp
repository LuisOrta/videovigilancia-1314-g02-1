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

#define VERSION 1



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(NULL)
{
    QSettings sett;
    ui->setupUi(this);
    ui->checkBox->setChecked(sett.value("Autorun", true).toBool());

    devices = QCamera::availableDevices();
    cam = new QCamera();
    isInicio = true;

    socket = new QTcpSocket(this);
    cierre = 0;

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
    int protocol_version = 1;

    QPixmap frame;
    frame.convertFromImage(rect,Qt::AutoColor);
    ui->label->setPixmap(frame);

    //Envio de la imagen por socket
    QBuffer buffer;
    QImageWriter writer(&buffer, "jpeg");
    writer.setCompression(70);
    writer.write(rect);
    qDebug() << QByteArray::number(buffer.data().size());
    int size = buffer.data().size();
    socket->read(reinterpret_cast<char*>(&cierre), 4);
    if (cierre == 202){
        qDebug() << "DESCONECTADO";
        socket->disconnectFromHost();
    }
    else{
        qDebug() << "ENVIANDO";
        socket->write(reinterpret_cast<char*>(&size), 4);
        socket->write(reinterpret_cast<char*>(&protocol_version), 4);
        socket->write(buffer.data());
    }
}






void MainWindow::on_actionCapturar_desde_WebCam_triggered()
{
    QSettings sett;
    qDebug() << sett.value("IPServer", "127.0.0.1").toString();
    socket->connectToHost(sett.value("IPServer", "127.0.0.1").toString(),sett.value("IPPort", 15000).toInt());


    if (socket->waitForConnected()==true){

        if(cam->state() != QCamera::ActiveState || isInicio){
            //Crear la camara y el visor
            for(int i=0; i<devices.length();i++){
                if(devices[i]== sett.value("Camera", "/dev/video0")){
                    cam = new QCamera(devices[i]);
                    isInicio = false;
                }
            }

            buffer = new capturebuffer();

            //Establecer buffer como visor de la cam
            cam->setViewfinder(buffer);
            cam->setCaptureMode(QCamera::CaptureViewfinder);
            cam->start();
            //Conectar la señal mostrar de present() con el slot de ShowFrame()
            connect(buffer, SIGNAL(mostrar(const QImage&)), this, SLOT(showFrame(const QImage&)));
        }

        else{
            cam->stop();
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
}
