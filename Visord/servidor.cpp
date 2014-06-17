#include "servidor.h"

Servidor::Servidor(QObject *parent) :
    QObject(parent),
    server_(NULL),
    handle_(NULL)
{
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);

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

    hash_clientsconnections.clear();
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
    if(!hash_clientsconnections.contains(clientConnection))
    {
        flags_protocols estructura_flags;
        estructura_flags.buffsize_ = 0;
        estructura_flags.cont_rect_ = 0;
        estructura_flags.n_camara_ = 0;
        estructura_flags.n_rec_ = 0;
        estructura_flags.protocol_version_ = 0;
        estructura_flags.terminado_roi_ = false;
        hash_clientsconnections.insert(clientConnection,estructura_flags);
    }

    qDebug() << "LEYENDO DATOS!";
    while(clientConnection != NULL && clientConnection->bytesAvailable()>=4){
        if (hash_clientsconnections[clientConnection].buffsize_ == 0){
            if (clientConnection->bytesAvailable()>=4){//TAMAÑO
                u_int32_t buffsize;
                clientConnection->read(reinterpret_cast<char*>(&buffsize), sizeof(buffsize));
                buffsize = qToLittleEndian(buffsize);
                hash_clientsconnections[clientConnection].buffsize_ = buffsize;
            }
            if (hash_clientsconnections[clientConnection].buffsize_ > 50000 || hash_clientsconnections[clientConnection].buffsize_  == 1){
                if(clientConnection->state() == QTcpSocket::ConnectedState)
                {
                    if(clientConnection->isWritable())
                    {
                     clientConnection->write(reinterpret_cast<char*>(&cerrar), sizeof(cerrar));
                    }
                    clientConnection->disconnectFromHost();
                    //clientConnection->abort();
                    hash_clientsconnections[clientConnection].readbuffer_.clear();
                    hash_clientsconnections[clientConnection].buffsize_  = 0;
                    hash_clientsconnections[clientConnection].protocol_version_ = 0;
                    hash_clientsconnections[clientConnection].n_rec_ = 0;
                    hash_clientsconnections[clientConnection].terminado_roi_ = false;
                    hash_clientsconnections[clientConnection].n_camara_ = 0;
                    return;

                }
            }
         }

        while (hash_clientsconnections[clientConnection].protocol_version_ == 0){

            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && hash_clientsconnections[clientConnection].protocol_version_ == 0){ //VERSION
                u_int32_t protocol_version;
                clientConnection->read(reinterpret_cast<char*>(&protocol_version), sizeof(protocol_version));
                protocol_version = qToLittleEndian(protocol_version);
                hash_clientsconnections[clientConnection].protocol_version_ = protocol_version;


                if (hash_clientsconnections[clientConnection].protocol_version_ != VERSION){
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


        while (hash_clientsconnections[clientConnection].n_camara_ == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && hash_clientsconnections[clientConnection].n_camara_ == 0){ //NUMERO DE CAMARA
                int n_camara;
                clientConnection->read(reinterpret_cast<char*>(&n_camara), sizeof(n_camara));
                n_camara = qToLittleEndian(n_camara);
                hash_clientsconnections[clientConnection].n_camara_ = n_camara;

            }
          }


        while (hash_clientsconnections[clientConnection].n_rec_ == 0){
            if (clientConnection != NULL && clientConnection->bytesAvailable()>=4 && hash_clientsconnections[clientConnection].n_rec_ == 0){ //NUMERO DE RECTANGULOS
                u_int32_t  n_rec;
                clientConnection->read(reinterpret_cast<char*>(&n_rec), sizeof(n_rec));
                n_rec= qToLittleEndian(n_rec);
                hash_clientsconnections[clientConnection].n_rec_ = n_rec;

            }
        }


        while(hash_clientsconnections[clientConnection].cont_rect_ <  hash_clientsconnections[clientConnection].n_rec_ &&  hash_clientsconnections[clientConnection].terminado_roi_ == false){

            if (clientConnection != NULL  && clientConnection->bytesAvailable()>=16){
                rect aux_rect;
                clientConnection->read(reinterpret_cast<char*>(&aux_rect.x), sizeof(aux_rect.x));
                aux_rect.x= qToLittleEndian(aux_rect.x);


                clientConnection->read(reinterpret_cast<char*>(&aux_rect.y), sizeof(aux_rect.y));
                aux_rect.y= qToLittleEndian(aux_rect.y);


                clientConnection->read(reinterpret_cast<char*>(&aux_rect.w), sizeof(aux_rect.w));
                aux_rect.w= qToLittleEndian(aux_rect.w);


                clientConnection->read(reinterpret_cast<char*>(&aux_rect.h), sizeof(aux_rect.h));
                aux_rect.h= qToLittleEndian(aux_rect.h);

                hash_clientsconnections[clientConnection].v_rect_.push_back(aux_rect);

                hash_clientsconnections[clientConnection].cont_rect_++;
            }

        }
        hash_clientsconnections[clientConnection].cont_rect_ = 0;
        hash_clientsconnections[clientConnection].terminado_roi_ = true;

        while(hash_clientsconnections[clientConnection].readbuffer_.length() < hash_clientsconnections[clientConnection].buffsize_ && clientConnection != NULL && clientConnection->bytesAvailable()>0){
            if (hash_clientsconnections[clientConnection].readbuffer_.length() < hash_clientsconnections[clientConnection].buffsize_){
                 hash_clientsconnections[clientConnection].readbuffer_.append(clientConnection->read(hash_clientsconnections[clientConnection].buffsize_ - hash_clientsconnections[clientConnection].readbuffer_.length()));

            }

            if(hash_clientsconnections[clientConnection].readbuffer_.length() == hash_clientsconnections[clientConnection].buffsize_){
                 QFile file("imagenCargada.jpg");
                 file.open(QIODevice::WriteOnly);
                 file.write(hash_clientsconnections[clientConnection].readbuffer_, hash_clientsconnections[clientConnection].buffsize_);
                 bool carga = imagebuff_.loadFromData(hash_clientsconnections[clientConnection].readbuffer_.data(), "jpeg");

                 /*-------------------------------------------------------*/
                 /*FUNCION DE ALMACENAR METADATOS (STD::VECTOR)*/
                 /*VACIAR EL VECTOR EN LA FUNCION CUANDO SE TENGA*/
                 almacenar_metadatos(hash_clientsconnections[clientConnection].n_camara_,hash_clientsconnections[clientConnection].v_rect_);
                 while(!hash_clientsconnections[clientConnection].v_rect_.empty()){
                    hash_clientsconnections[clientConnection].v_rect_.pop_back();
                 }
                 /*-------------------------------------------------------*/
                 //showFrame(imagebuff);
                 hash_clientsconnections[clientConnection].readbuffer_.clear();
                 hash_clientsconnections[clientConnection].buffsize_ = 0;
                 hash_clientsconnections[clientConnection].protocol_version_ = 0;
                 hash_clientsconnections[clientConnection].n_rec_ = 0;
                 hash_clientsconnections[clientConnection].terminado_roi_ = false;
                 hash_clientsconnections[clientConnection].n_camara_ = 0;

            }
        }
    }

}
void Servidor::clienteDesconectado()
{

    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection)
        return;

    syslog(LOG_NOTICE,"CLIENTE DESCONECTADO");
    clientConnections_.removeAll(clientConnection);
    clientConnection->deleteLater();
    hash_clientsconnections[clientConnection].readbuffer_.clear();
}
void Servidor::reiniciar()
{
    hash_clientsconnections.clear();
    if(server_ != NULL && server_->isListening())
    {
         server_->close();
         qDeleteAll(clientConnections_);
    }
    escucha();

}
void Servidor::cerrar()
{
    if(server_ != NULL && server_->isListening())
     {

        server_->close();
        qDeleteAll(clientConnections_);

      }
    hash_clientsconnections.clear();

    syslog(LOG_NOTICE," DEMONIO DETENIDO ");
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



