#include <QCoreApplication>
#include <servidor.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <QString>
#include <stdio.h>
#include <QSettings>
#define FICHERO_PID "/var/run/Visord.pid"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Luis");
    QCoreApplication::setApplicationName("Visor");

    pid_t pid;
    pid_t sid;




    // Creamos un proceso
    pid = fork();

    // Si pid es < 0, fork() falló
    if (pid < 0) {
        // Mostrar la descripción del error y terminar
        std::cerr << strerror(errno) << std::endl;
        exit(10);
    }

    // Si pid es > 0, estamos en el proceso padre
    if (pid > 0) {

        // Terminar el proceso padre  para demonizar el hijo  y que este quede como hijo de init
        exit(0);
    }
    // Proceso hijo demonizado en este caso se instacia solamente el objeto con un bucle de mensajes propio del objeto

    Servidor Servidord;
    umask(0);
    QSettings sett("ficheroServer.ini", QSettings::IniFormat);
    QString cadena = sett.value("Usuario_efectivo","Visord").toString();
    struct stat st;
    FILE *fichero_demonio;


    if(stat(FICHERO_PID,&st) == 0)
    {
        syslog(LOG_ERR,"Ya existe un proceso igual cargado");
        exit(11);

    }
    fichero_demonio = fopen(FICHERO_PID,"w");
    if(fichero_demonio == NULL)
     {
        syslog(LOG_ERR,"No se pudo crear el fichero PID ");
        exit(14);
    }

    fprintf(fichero_demonio,"%d\n",getpid());
    fclose(fichero_demonio);

    // Cambiar el usuario efectivo del proceso
    passwd* user = getpwnam(cadena.toLocal8Bit());

    if(user == NULL)
     {
        syslog(LOG_ERR, "No fue posible establecer un usuario efectivo \n");
        unlink(FICHERO_PID);
        exit (13);

     }
    seteuid(user->pw_uid);
    cadena = sett.value("Grupo_efectivo","Visord").toString();
    // Cambiar el grupo efectivo del proceso
    group* group = getgrnam(cadena.toLocal8Bit());
    if(group == NULL)
    {
            syslog(LOG_ERR, "No fue posible establecer un grupo efectivo \n");
            unlink(FICHERO_PID);
            exit (13);
    }
    setegid(group->gr_gid);
    openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "Demonio iniciado con éxito\n");
    // Intentar crear una nueva sesión
    sid = setsid();
    if (sid < 0) {
        syslog(LOG_ERR, "No fue posible crear una nueva sesión\n");
        unlink(FICHERO_PID);
        exit(11);
        // Error 11 si no se pudo cambiar de sesíon al proceso hijo
    }
    // Cambiar el directorio de trabajo de proceso
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "No fue posible cambiar el directorio de "
                       "trabajo a /\n");
        unlink(FICHERO_PID);
        exit(12);
    }
    // Cerrar los descriptores de la E/S estándar y reabrirlos para volcar la información de los sockets a el fichero vacío
    close(STDIN_FILENO);                    // fd 0
    close(STDOUT_FILENO);                   // fd 1
    close(STDERR_FILENO);                   // fd 2


    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2
    closelog();
    return a.exec();



}
