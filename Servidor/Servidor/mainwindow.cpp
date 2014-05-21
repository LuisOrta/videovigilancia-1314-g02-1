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
#include "QtSql/qsqlquery.h"
#include "QDateTime"

#define VERSION 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(NULL)
{
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    ui->setupUi(this);
    ui->checkBox->setChecked(sett.value("Autorun", true).toBool());

    if(sett.value("Contador").toInt()!=0)
        contador_ = sett.value("Contador").toInt();

    else
        contador_ = 0;

     qDebug()<<"Contador";
     qDebug()<<contador_;
    devices = QCamera::availableDevices();
    cam = new QCamera();
    isInicio = true;
    buffsize = 0;
    corrupt_counter = 0;
    cont_rect = 0;
    protocol_version = 0;
    n_rec = 0;
    terminado_roi = false;
    n_camara = 0;
    server = new QTcpServer(this);
    if(sett.value("Autorun",true).toBool()==true)
    {
      server->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
      connect(server, SIGNAL(newConnection()), this, SLOT(escucha()));
    }
      db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("data.sqlite");
      db.open();

}

MainWindow::~MainWindow()
{
    if(server->isListening())
    {
        qDeleteAll(clientConnections);

    }
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
            QTcpSocket *clientConnection = server->nextPendingConnection();
            corrupt_counter = 0;
            connect(clientConnection, SIGNAL(readyRead()), this, SLOT(leerDatos()));
            connect(clientConnection, SIGNAL(disconnected()), this, SLOT(clienteDesconectado()));
            clientConnections.append(clientConnection);
    }

}


void MainWindow::leerDatos()
{
     QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    u_int32_t cerrar = 202;
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection)
        return;

    qDebug() << "LEYENDO DATOS!";
    while(clientConnection != NULL && clientConnection->bytesAvailable()>=4){
        if (buffsize == 0){
            if (clientConnection->bytesAvailable()>=4){//TAMAÑO
                clientConnection->read(reinterpret_cast<char*>(&buffsize), sizeof(buffsize));
                buffsize = qToLittleEndian(buffsize);
                qDebug() << "LEYENDO TAMAÑO!";
                qDebug() << buffsize;
            }
            if (buffsize > 50000 || buffsize == 1){
                if(clientConnection->state() == QTcpSocket::ConnectedState)
                {
                    if(clientConnection->isWritable())
                    {
                     clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                    }
                    clientConnection->disconnectFromHost();
                    //clientConnection->abort();
                    readbuffer.clear();
                    buffsize = 0;
                    protocol_version = 0;
                    n_rec = 0;
                    terminado_roi = false;
                    n_camara = 0;
                    return;

                }
            }
         }

        while (protocol_version == 0){
            qDebug() << " ERROR INFINITO " << endl;
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && protocol_version == 0){ //VERSION
                clientConnection->read(reinterpret_cast<char*>(&protocol_version), sizeof(protocol_version));
                protocol_version = qToLittleEndian(protocol_version);
                qDebug() << "VERSION!";
                qDebug() << protocol_version;
                if (protocol_version != VERSION){
                    if(clientConnection->state() == QTcpSocket::ConnectedState)
                    {
                        if(clientConnection->isWritable())
                        {
                         clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                        }
                       clientConnection->disconnectFromHost();
                    }
                }

            }
          }


        while (n_camara == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && n_camara == 0){ //NUMERO DE CAMARA
                clientConnection->read(reinterpret_cast<char*>(&n_camara), sizeof(n_camara));
                n_camara = qToLittleEndian(n_camara );
                qDebug() << "ID de CAMARA!";
                qDebug() << n_camara;
            }
          }


        while (n_rec == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && n_rec == 0){ //NUMERO DE RECTANGULOS
                clientConnection->read(reinterpret_cast<char*>(&n_rec), sizeof(n_rec));
                n_rec= qToLittleEndian(n_rec);
                qDebug() << "N de RECTANGULOS";
                qDebug() << n_rec;
            }
        }


        while(cont_rect < n_rec && terminado_roi == false){
            qDebug() << "N_ROI!";
            if (clientConnection != NULL  && clientConnection->bytesAvailable()>=16){
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

        while(readbuffer.length() < buffsize && clientConnection != NULL && clientConnection->bytesAvailable()>0){
            if (readbuffer.length() < buffsize){
                 readbuffer.append(clientConnection->read(buffsize - readbuffer.length()));
                 qDebug() << "TAMAÑO BUFFER!";
                 qDebug() << readbuffer.length();
            }

            if(readbuffer.length() == buffsize){

                 qDebug()<<"Contadooor";
                 qDebug()<<contador_;
                 QString hex("%1");
                 QString hex2 = hex.arg(contador_, 20, 16, QLatin1Char('0'));
                 QString ruta1 = hex2.left(4);
                 QString ruta2 = hex2.mid(5,4);
                 ruta_.mkdir(ruta1);
                 ruta_.mkdir(ruta1+"/"+ruta2);
                 QString aux = APP_IMAGE;
                 qDebug()<<aux+"/"+ruta1+"/"+ruta2+"/"+hex2;

                 QFile file(aux+"/"+ruta1+"/"+ruta2+"/"+hex2);
                 contador_++;
                 sett.setValue("Contador", contador_);
                 bool prueba = file.open(QIODevice::WriteOnly);
                 if(!prueba){
                     qDebug()<<"Error al abrir la imagen";
                     return;

                 }
                 file.write(readbuffer, buffsize);
                 bool carga = imagebuff.loadFromData(readbuffer.data(), "jpeg");

                 /*-------------------------------------------------------*/
                 /*FUNCION DE ALMACENAR METADATOS (STD::VECTOR)*/
                 almacenar_metadatos(n_camara,v_rect);
                 while(!v_rect.empty()){
                    v_rect.pop_back();
                 }
                 /*-------------------------------------------------------*/
                 //showFrame(imagebuff);
                 readbuffer.clear();
                 buffsize = 0;
                 protocol_version = 0;
                 n_rec = 0;
                 terminado_roi = false;
                 n_camara = 0;

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

void MainWindow:: clienteDesconectado()
{
     QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
     if (!clientConnection)
         return;

     clientConnections.removeAll(clientConnection);
     clientConnection->deleteLater();
     readbuffer.clear();
     //qDebug() << "ENTRO";

}

void MainWindow::on_actionTerminar_Servidor_triggered()
{
    if(server->isListening())
    {

        server->close();
        qDeleteAll(clientConnections);
        readbuffer.clear();
        isInicio = true;
        buffsize = 0;
        corrupt_counter = 0;
        cont_rect = 0;
        protocol_version = 0;
        n_rec = 0;
        terminado_roi = false;
        n_camara = 0;
    }


}
void MainWindow::on_actionIniciar_Servidor_triggered()
{

    if(!server->isListening())
    {
        QSettings sett("ficheroServer.ini", QSettings::IniFormat);

        server->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
        connect(server, SIGNAL(newConnection()), this, SLOT(escucha()));
    }

}

void MainWindow::almacenar_metadatos(int nombre, std::vector<rect> metadatos)
{

   //FECHA Y HORA
   QDateTime datetime;
   datetime = QDateTime::currentDateTime();
   QDate date = datetime.date();
   QTime time = datetime.time();



   QSqlQuery query(db);
   query.exec("CREATE TABLE IF NOT EXISTS image"
              "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "name VARCHAR(50),"
              "day INTEGER,"
              "month INTEGER,"
              "year INTEGER,"
              "hour INTEGER,"
              "minute INTEGER)");

  query.exec("CREATE TABLE IF NOT EXISTS roi"
             "(x INTEGER,"
             "y INTEGER,"
             "w INTEGER,"
             "h INTEGER,"
             "id INTEGER,"
             "FOREIGN KEY(id) REFERENCES image(id))");

  query.prepare("INSERT INTO image(name, day, month, year, hour, minute)"
                "VALUES(:name, :day, :month, :year, :hour, :minute)");
  query.bindValue(":name", nombre);
  query.bindValue(":day", date.day());
  query.bindValue(":month", date.month());
  query.bindValue(":year", date.year());
  query.bindValue(":hour", time.hour());
  query.bindValue(":minute", time.minute());
  query.exec();

  query.exec("SELECT id FROM image");
  query.last();
  int ident = query.value(0).toInt();

  for(int i=0;i<metadatos.size();i++){
      query.prepare("INSERT INTO roi(x,y,w,h,id)"
                    "VALUES(:x, :y, :w, :h, :id)");
      query.bindValue(":x", metadatos[i].x);
      query.bindValue(":y", metadatos[i].y);
      query.bindValue(":w", metadatos[i].w);
      query.bindValue(":h", metadatos[i].h);
      query.bindValue(":id",ident);
      query.exec();
  }
  qDebug() << "Almacenado en SQLite";
}
