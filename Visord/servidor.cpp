#include "servidor.h"

Servidor::Servidor(QObject *parent) :
    QObject(parent),
    server_(NULL),
    handle_(NULL)
{
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    buffsize_ = 0;
    corrupt_counter_ = 0;
    cont_rect_ = 0;
    protocol_version_ = 0;
    n_rec_ = 0;
    n_camara_ = 0;
    terminado_roi_ = false;
    server_ = new QTcpServer(this);
    handle_ = new Manejador(this);

    signal(SIGINT,Manejador::intSignalHandler);
    signal(SIGHUP,Manejador::hupSignalHandler);
    signal(SIGTERM,Manejador::termSignalHandler);
    if(server_ != NULL && handle_ != NULL)
    {
        connect(handle_,SIGNAL(reiniciar()),this,SLOT(reiniciar()));
        connect(handle_,SIGNAL(terminar()),this,SLOT(cerrar()));

        server_->listen(QHostAddress::Any,sett.value("IPPort", 15000).toInt());
        connect(server_, SIGNAL(newConnection()), this, SLOT(escucha()));
        db_ = QSqlDatabase::addDatabase("QSQLITE");
        db_.setDatabaseName("data.sqlite");
        db_.open();
    }
}
Servidor::~Servidor()
{
    if(server_->isListening())
    {
        qDeleteAll(clientConnections_);

    }
    delete server_;
    delete handle_;
    server_ = NULL;
    handle_ = NULL;
}

void Servidor::escucha()
{

    while(server_->hasPendingConnections()){

            QTcpSocket *clientConnection = server_->nextPendingConnection();
            corrupt_counter_ = 0;
            connect(clientConnection, SIGNAL(readyRead()), this, SLOT(leerDatos()));
            connect(clientConnection, SIGNAL(disconnected()), this, SLOT(clienteDesconectado()));
            clientConnections_.append(clientConnection);
    }
}
void Servidor::leerDatos()
{
    u_int32_t cerrar = 202;
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection)
        return;

    qDebug() << "LEYENDO DATOS!";
    while(clientConnection != NULL && clientConnection->bytesAvailable()>=4){
        if (buffsize_ == 0){
            if (clientConnection->bytesAvailable()>=4){//TAMAÑO
                clientConnection->read(reinterpret_cast<char*>(&buffsize_), sizeof(buffsize_));
                buffsize_ = qToLittleEndian(buffsize_);
                qDebug() << "LEYENDO TAMAÑO!";
                qDebug() << buffsize_;
            }
            if (buffsize_ > 50000 || buffsize_ == 1){
                if(clientConnection->isWritable())
                {
                    clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                }
                clientConnection->disconnectFromHost();
                readbuffer_.clear();
                buffsize_ = 0;
                cont_rect_ = 0;
                protocol_version_ = 0;
                n_rec_ = 0;
                terminado_roi_ = false;
                n_camara_ = 0;
                return;
            }
         }

        while (protocol_version_ == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && protocol_version_ == 0){ //VERSION
                clientConnection->read(reinterpret_cast<char*>(&protocol_version_), sizeof(protocol_version_));
                protocol_version_ = qToLittleEndian(protocol_version_);
                qDebug() << "VERSION!";
                qDebug() << protocol_version_;
                if (protocol_version_ != VERSION){
                    if(clientConnection->isWritable())
                    {
                        clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                    }
                    clientConnection->disconnectFromHost();
                }
            }
          }


        while (n_camara_ == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && n_camara_ == 0){ //NUMERO DE CAMARA
                clientConnection->read(reinterpret_cast<char*>(&n_camara_), sizeof(n_camara_));
                n_camara_ = qToLittleEndian(n_camara_ );
                qDebug() << "ID de CAMARA!";
                qDebug() << n_camara_;
            }
          }


        while (n_rec_ == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && n_rec_ == 0){ //NUMERO DE RECTANGULOS
                clientConnection->read(reinterpret_cast<char*>(&n_rec_), sizeof(n_rec_));
                n_rec_= qToLittleEndian(n_rec_);
                qDebug() << "N de RECTANGULOS";
                qDebug() << n_rec_;
            }
        }


        while(cont_rect_ < n_rec_ && terminado_roi_ == false){
            qDebug() << "N_ROIS!";
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=16){
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

                v_rect_.push_back(aux_rect);

                cont_rect_++;
            }

        }
        cont_rect_ = 0;
        terminado_roi_ = true;

        while(readbuffer_.length() < buffsize_ && clientConnection != NULL && clientConnection->bytesAvailable()>0){
            if (readbuffer_.length() < buffsize_){
                 readbuffer_.append(clientConnection->read(buffsize_ - readbuffer_.length()));
                 qDebug() << "TAMAÑO BUFFER!";
                 qDebug() << readbuffer_.length();
            }

            if(readbuffer_.length() == buffsize_){
                 QFile file("imagenCargada.jpg");
                 file.open(QIODevice::WriteOnly);
                 file.write(readbuffer_, buffsize_);
                 file.close();

                 /*-------------------------------------------------------*/
                 /*FUNCION DE ALMACENAR METADATOS (STD::VECTOR)*/
                 /*VACIAR EL VECTOR EN LA FUNCION CUANDO SE TENGA*/
                 while(!v_rect_.empty()){
                    v_rect_.pop_back();
                 }

                 readbuffer_.clear();
                 buffsize_ = 0;
                 protocol_version_ = 0;
                 n_rec_ = 0;
                 terminado_roi_ = false;

            }
        }
    }

}
void Servidor::clienteDesconectado()
{

    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection)
        return;
    clientConnections_.removeAll(clientConnection);
    readbuffer_.clear();
    qDebug() << "ENTRO";
    clientConnection->deleteLater();

}
void Servidor::reiniciar()
{
    if(server_ != NULL && server_->isListening())
    {
         server_->close();
         qDeleteAll(clientConnections_);
    }
    readbuffer_.clear();
    buffsize_ = 0;
    cont_rect_ = 0;
    protocol_version_ = 0;
    n_rec_ = 0;
    n_camara_ = 0;
    terminado_roi_ = false;
    escucha();

}
void Servidor::cerrar()
{
    if(server_ != NULL && server_->isListening())
     {

        server_->close();
        qDeleteAll(clientConnections_);
        readbuffer_.clear();
    }
    exit(0);
}
void Servidor::almacenar_metadatos(int nombre, std::vector<rect> metadatos)
{

   //FECHA Y HORA
   QDateTime datetime;
   datetime = QDateTime::currentDateTime();
   QDate date = datetime.date();
   QTime time = datetime.time();



   QSqlQuery query(db_);
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



