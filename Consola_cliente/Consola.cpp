#include "Consola.h"

Consola::Consola(QObject *parent):
    QObject(parent),
    qtout_(NULL),
    qtin_(NULL),
    hilo_captura_(NULL),
    objeto_captura_(NULL),
    socket_(NULL)
{
    qtout_ = new QTextStream(stdout,QIODevice::WriteOnly);
    qtin_ = new QTextStream(stdin,QIODevice::ReadOnly);
    ip_ = conf_.value("Visor/ip","127.0.0.1").toString();
    puerto_ = conf_.value("Visor/puerto","15000").toString();
    socket_ = new QTcpSocket(this);
    dispositivos_ =  QCamera::availableDevices();
    hilo_captura_ = new QThread(this);
    handle_ = new Manejador(this);
     connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(displayError(QAbstractSocket::SocketError)));


     signal(SIGINT,Manejador::intSignalHandler);
     signal(SIGHUP,Manejador::hupSignalHandler);
     signal(SIGTERM,Manejador::termSignalHandler);

     connect(handle_,SIGNAL(terminar()),this,SLOT(cerrar_objetos()));

     *qtout_ << " SIN PROBLEMAS " << endl;
     qtout_->flush();
     mostrar_menu();


}
Consola::~Consola()
{
    if (hilo_captura_ != NULL)
    {
        if (hilo_captura_->isRunning())
        {
            hilo_captura_->quit();
            hilo_captura_->wait();
        }
    }
    qtout_->flush();
    qtin_ ->flush();

    delete  socket_;
    delete  qtout_;
    delete  qtin_;
    delete  hilo_captura_;
    delete  objeto_captura_;
    delete  handle_;



    qtout_ = NULL;
    qtin_ = NULL;
    socket_ = NULL;
    hilo_captura_ = NULL;
    objeto_captura_= NULL;
    handle_ = NULL;
}




void Consola::mostrar_menu(void)
{
    qtout_->flush();
    eleccion_.clear();
    dispositivo_sel_ =  conf_.value("Visor/camera").toString();
    *qtout_ << "\n\nBienvenido  a la Consola Cliente de Visor" << endl;
    *qtout_ << " Version 1.0" << endl;
    *qtout_ << " Basado en el entorno Qt 5.2.0 (GCC 4.6.1.32bit) " << endl;
    *qtout_ << " Desarrollado por : " << endl;
    *qtout_  << " Proyecto empezado el 11 de Febrero del 2014 \n por :\n\n  Diaz Marrero Juan Jose Gregorio \n  Hernandez Diaz Daura \n  Orta Mendez Luis Antonio\n ";
    *qtout_  << "\n\n\n";
    *qtout_  << " --------------------------------------- " << endl;
    *qtout_  << " -            MENU VISOR CLIENTE       - " << endl;
    *qtout_  << " --------------------------------------- " << endl;
    *qtout_  << " 1- Configurar conexion " << endl;
    *qtout_  << " 2- Configurar dispositivo de grabacion de video actual (" << dispositivo_sel_ << ")" <<endl;
    *qtout_  << " 3- Menu transmitir frames " << endl;
    *qtout_  << " C|c- Cerrar Consola " << endl;


     *qtout_<< "  \n\n Por favor, seleccione una de las opciones " << endl;

     eleccion_ = qtin_->readLine();

     if(eleccion_[0] !='1' && eleccion_[0] != '2' && eleccion_[0] !='3' && eleccion_[0] != 'c' && eleccion_[0] != 'C')
     {
            mostrar_menu();
     }
     else
     {
           if (eleccion_[0] == '1')
           {

               mostrar_menu_configuracion();

           }
           else if (eleccion_[0] == '2')
           {
                mostrar_menu_configuracion_dispositivo_video();
           }
           else if (eleccion_[0] == '3')
           {
                mostrar_menu_capturar();
           }
           else if (eleccion_[0] == 'c' || eleccion_[0] == 'C')
           {
               exit(0);
           }
      }

}
void Consola::mostrar_menu_configuracion(void)
{
     qtout_->flush();
     eleccion_.clear();
     *qtout_  << " -------------------------------------------------------- " << endl;
     *qtout_  << " -            MENU CONFIGURACION CONEXION CLIENTE       - " << endl;
     *qtout_  << " -------------------------------------------------------- " << endl;
     *qtout_  << " 1- Configurar Ip (Por defecto 127.0.0.1) actual ( " << ip_ <<" )" << endl;
     *qtout_  << " 2- Configurar Puerto (Por defecto 15000) actual ( " << puerto_ <<") " << endl;
     *qtout_  << " V|v- Volver al menu anterior" << endl;
     *qtout_  << " C|c- Cerrar aplicacion" << endl;
     *qtout_ << "  \n\n Por favor, seleccione una de las opciones " << endl;
     eleccion_ = qtin_->readLine();
     if(eleccion_[0] != '1' &&  eleccion_[0] != '2' && eleccion_[0] != 'V' && eleccion_[0] != 'v' && eleccion_[0] != 'C' && eleccion_[0] != 'c')
     {
            mostrar_menu_configuracion();
     }
     else
     {
           if (eleccion_[0] == '1')
           {
                mostrar_menu_configuracion_ip();
           }
           else if (eleccion_[0] == '2')
           {
                mostrar_menu_configuracion_puerto();
           }
           else if (eleccion_[0] == 'V' || eleccion_[0] == 'v')
           {

                mostrar_menu();
           }
           else if ( eleccion_[0] == 'C' || eleccion_[0] == 'c')
           {
               exit(0);
           }
    }


}
void Consola::mostrar_menu_configuracion_ip(void)
{

    qtout_->flush();
    QString respuesta;
    respuesta.clear();
    *qtout_ << " Introduzca su ip : " << endl;
     ip_ = qtin_->readLine();
     *qtout_ << " Desea guardar esta nueva configuración Si(s|S)/ No (n|N) : " << endl;
      respuesta = qtin_->readLine();
      if(respuesta[0] == 's'|| respuesta[0] == 'S')
      {
          conf_.setValue("Visor/ip",ip_);
          conf_.sync();
      }
      mostrar_menu_configuracion();


}
void Consola::mostrar_menu_configuracion_puerto(void)
{
    qtout_->flush();
    QString respuesta;
    respuesta.clear();
    *qtout_ << " Introduzca el puerto  : " << endl;
    puerto_ = qtin_->readLine();
    *qtout_ << " Desea guardar esta nueva configuración Si(s|S)/ No (n|N) : " << endl;
    respuesta = qtin_->readLine();
    if(respuesta[0] == 's'|| respuesta[0] == 'S')
    {
          conf_.setValue("Visor/puerto",puerto_);
          conf_.sync();
     }
     mostrar_menu_configuracion();
}
void Consola::mostrar_menu_configuracion_dispositivo_video(void)
{
    eleccion_.clear();

    *qtout_  << " -------------------------------------------------------------------- " << endl;
    *qtout_  << " -            MENU CONFIGURACION DISPOSITIVO DE GRABACION           - " << endl;
    *qtout_  << " -------------------------------------------------------------------- " << endl;
    for(int i=0;i < dispositivos_.length();i++)
     {
          *qtout_<<" " << i << "- " << dispositivos_[i]<< endl;
     }

    *qtout_  << " V|v- Volver al menu anterior" << endl;
    *qtout_  << " C|c- Cerrar aplicacion" << endl;
    *qtout_ << "  \n\n Por favor, seleccione una de las opciones " << endl;
     eleccion_  = qtin_->readLine();
     bool seleccionado = false;
     QString aux = "";
     int iterador_seleccionado = -1;
     for(int i=0;i < dispositivos_.length() ;i++)
     {
             aux.clear();
             aux.setNum(i);
             if ( eleccion_.compare(aux) == 0)
             {
                seleccionado = true;
                iterador_seleccionado = i;
             }

     }
     if (seleccionado == false && (eleccion_[0] == 'c' || eleccion_[0] == 'C'))
     {
         exit(0);
     }
     else if(seleccionado == false && (eleccion_[0] == 'v' || eleccion_[0] == 'V'))
     {
         mostrar_menu();
     }
     else if(seleccionado == false)
     {
            mostrar_menu_configuracion_dispositivo_video();
     }
     else
     {
         QString respuesta;
         respuesta.clear();
         conf_.setValue("Visor/camara",dispositivos_[iterador_seleccionado]);
         dispositivo_sel_ = dispositivos_[iterador_seleccionado];
         conf_.sync();
         qtout_->flush();
         mostrar_menu_configuracion();
     }

}

void Consola::displayError(QAbstractSocket::SocketError socketError)
{
    *qtout_ << "------------------------------------------" << endl;
    *qtout_ << "- ADVERTENCIA - SE HA PRODUCIDO UN ERROR -" << endl;
    *qtout_ << "------------------------------------------" << endl;

   switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:

        *qtout_ << "Se ha cerrado la conexion con el host remoto" << endl;
        // Si se cae la conexión espero a que se terminen de procesar los mensajes de hilo y elimino el objeto capturar lo que implica cerrar el socket
       break;
    case QAbstractSocket::HostNotFoundError:

        *qtout_ << "El host no se ha encontrado. Por favor, revise la dirección del host y el puerto en la configuración." << endl;
        break;

    case QAbstractSocket::ConnectionRefusedError:
        *qtout_ << " No se ha podido establecer la conexion " << endl;
        break;
    default:
        *qtout_ << " Ha ocurrido el siguiente error " << socket_->errorString() <<endl;
    }


   if(hilo_captura_->isRunning())
   {

       hilo_captura_->quit();
       hilo_captura_->wait();

   }
   socket_->abort();
   *qtout_ << "\n";
   qtout_->flush();
   mostrar_menu_capturar();


}
void Consola::mostrar_menu_capturar(void)
{

    qtout_->flush();
    eleccion_.clear();
    *qtout_  << " -----------------------------------------------" << endl;
    *qtout_  << " -            MENU TRANSMITIR FRAMES           -" << endl;
    *qtout_  << " -----------------------------------------------" << endl;
    *qtout_  << " 1- Empezar a transmitir marcos" << endl;
    *qtout_  << " V|v- Volver al menu principal" << endl;
    *qtout_  << " \n";

    *qtout_  << " NOTA : Presione Control C para terminar Captura \n  y para volver al menu principal " << endl;

    eleccion_ = qtin_->readLine();



    if (eleccion_[0] == '1')
    {

            socket_->connectToHost(ip_,puerto_.toInt());
            if(socket_->waitForConnected() == true)
            {
                if(socket_->state() == 3)
                 {
                    //qRegisterMetaType< QList<QByteArray> >("QList<QByteArray>");
                    objeto_captura_ = new Captura(dispositivos_,socket_);
                    objeto_captura_->moveToThread(hilo_captura_);
                    if(!hilo_captura_->isRunning())
                    {
                        hilo_captura_->start();
                    }
                    connect( hilo_captura_, SIGNAL(started()),objeto_captura_, SLOT(empezar_capturar()) );
                    connect( hilo_captura_, SIGNAL(finished()),objeto_captura_, SLOT(deleteLater()) );
                }
            }


    }
    else if (eleccion_[0] == 'V' || eleccion_[0] == 'v')
    {
           mostrar_menu();
    }
    else
    {
        mostrar_menu_capturar();
    }

}
void Consola::cerrar_objetos(void)
{
    if(hilo_captura_->isRunning())
    {
        hilo_captura_->quit();
        hilo_captura_->wait();
    }

    socket_->abort();
    mostrar_menu();
}

