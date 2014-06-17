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

    devices = QCamera::availableDevices();
    cam = new QCamera();

    isInicio = true;
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
    u_int32_t cerrar = 202;
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection)
        return;

    if(!hash_clientsconnections.contains(clientConnection))
    {
        flags_protocols estructura_flags;
        estructura_flags.buffsize = 0;
        estructura_flags.cont_rect = 0;
        estructura_flags.n_camara = 0;
        estructura_flags.n_rec = 0;
        estructura_flags.protocol_version = 0;
        estructura_flags.terminado_roi = false;
        hash_clientsconnections.insert(clientConnection,estructura_flags);
    }


    qDebug() << "LEYENDO DATOS!";
    while(clientConnection != NULL && clientConnection->bytesAvailable()>=4){
        if (hash_clientsconnections[clientConnection].buffsize == 0){
            if (clientConnection->bytesAvailable()>=4){//TAMAÑO
                u_int32_t buffsize = 0;
                clientConnection->read(reinterpret_cast<char*>(&buffsize), sizeof(buffsize));
                buffsize = qToLittleEndian(buffsize);
                hash_clientsconnections[clientConnection].buffsize = buffsize;
                qDebug() << "LEYENDO TAMAÑO!";
                qDebug() << buffsize;
            }
            if ( hash_clientsconnections[clientConnection].buffsize  > 50000 || hash_clientsconnections[clientConnection].buffsize == 1){
                if(clientConnection->state() == QTcpSocket::ConnectedState)
                {
                    if(clientConnection->isWritable())
                    {
                     clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                    }
                    clientConnection->disconnectFromHost();
                    //clientConnection->abort();
                     hash_clientsconnections[clientConnection].readbuffer.clear();
                     hash_clientsconnections[clientConnection].buffsize = 0;
                     hash_clientsconnections[clientConnection].protocol_version = 0;
                     hash_clientsconnections[clientConnection].n_rec = 0;
                     hash_clientsconnections[clientConnection].terminado_roi = false;
                     hash_clientsconnections[clientConnection].n_camara = 0;
                    return;

                }
            }
         }

        while ( hash_clientsconnections[clientConnection].protocol_version == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 &&  hash_clientsconnections[clientConnection].protocol_version == 0){ //VERSION
                u_int32_t protocol_version = 0;
                clientConnection->read(reinterpret_cast<char*>(&protocol_version), sizeof(protocol_version));
                protocol_version = qToLittleEndian(protocol_version);
                hash_clientsconnections[clientConnection].protocol_version = protocol_version;
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


        while ( hash_clientsconnections[clientConnection].n_camara == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 &&  hash_clientsconnections[clientConnection].n_camara == 0){ //NUMERO DE CAMARA
                int n_camara;
                clientConnection->read(reinterpret_cast<char*>(&n_camara), sizeof(n_camara));
                n_camara = qToLittleEndian(n_camara );
                 hash_clientsconnections[clientConnection].n_camara = n_camara;
                qDebug() << "ID de CAMARA!";
                qDebug() << n_camara;
            }
          }


        while ( hash_clientsconnections[clientConnection].n_rec == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 &&  hash_clientsconnections[clientConnection].n_rec == 0){ //NUMERO DE RECTANGULOS
                u_int32_t n_rec;
                clientConnection->read(reinterpret_cast<char*>(&n_rec), sizeof(n_rec));
                n_rec= qToLittleEndian(n_rec);
                 hash_clientsconnections[clientConnection].n_rec = n_rec;
                qDebug() << "N de RECTANGULOS";
                qDebug() << n_rec;
            }
        }


        while( hash_clientsconnections[clientConnection].cont_rect <  hash_clientsconnections[clientConnection].n_rec &&  hash_clientsconnections[clientConnection].terminado_roi == false){
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

                 hash_clientsconnections[clientConnection].v_rect.push_back(aux_rect);

                 hash_clientsconnections[clientConnection].cont_rect++;
            }

        }
         hash_clientsconnections[clientConnection].cont_rect = 0;
         hash_clientsconnections[clientConnection].terminado_roi = true;

        while( hash_clientsconnections[clientConnection].readbuffer.length() <  hash_clientsconnections[clientConnection].buffsize && clientConnection != NULL && clientConnection->bytesAvailable()>0){
            if ( hash_clientsconnections[clientConnection].readbuffer.length() <  hash_clientsconnections[clientConnection].buffsize){
                  hash_clientsconnections[clientConnection].readbuffer.append(clientConnection->read( hash_clientsconnections[clientConnection].buffsize -  hash_clientsconnections[clientConnection].readbuffer.length()));
                 qDebug() << "TAMAÑO BUFFER!";
                 qDebug() <<  hash_clientsconnections[clientConnection].readbuffer.length();
            }

            if( hash_clientsconnections[clientConnection].readbuffer.length() ==  hash_clientsconnections[clientConnection].buffsize){
                 QFile file("imagenCargada.jpg");
                 file.open(QIODevice::WriteOnly);
                 file.write( hash_clientsconnections[clientConnection].readbuffer,  hash_clientsconnections[clientConnection].buffsize);
                 bool carga = imagebuff.loadFromData( hash_clientsconnections[clientConnection].readbuffer.data(), "jpeg");

                 /*-------------------------------------------------------*/
                 /*FUNCION DE ALMACENAR METADATOS (STD::VECTOR)*/
                 /*VACIAR EL VECTOR EN LA FUNCION CUANDO SE TENGA*/
                 almacenar_metadatos( hash_clientsconnections[clientConnection].n_camara, hash_clientsconnections[clientConnection].v_rect);
                 while(!hash_clientsconnections[clientConnection].v_rect.empty()){
                     hash_clientsconnections[clientConnection].v_rect.pop_back();
                 }
                 /*-------------------------------------------------------*/
                 //showFrame(imagebuff);
                  hash_clientsconnections[clientConnection].readbuffer.clear();
                  hash_clientsconnections[clientConnection].buffsize = 0;
                  hash_clientsconnections[clientConnection].protocol_version = 0;
                  hash_clientsconnections[clientConnection].n_rec = 0;
                  hash_clientsconnections[clientConnection].terminado_roi = false;
                  hash_clientsconnections[clientConnection].n_camara = 0;

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
      hash_clientsconnections[clientConnection].readbuffer.clear();


}

void MainWindow::on_actionTerminar_Servidor_triggered()
{
    if(server->isListening())
    {

        server->close();
        qDeleteAll(clientConnections);
        hash_clientsconnections.clear();

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

   query.exec("PRAGMA [data.sqlite.]synchronous = NORMAL");
   query.exec("PRAGMA [data.sqlite.]journal_mode = MEMORY");

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
