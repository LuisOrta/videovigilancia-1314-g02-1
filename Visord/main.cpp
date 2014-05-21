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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Luis");
    QCoreApplication::setApplicationName("Visor");
    pid_t pid;
    pid_t sid;

    // Nos clonamos a nosotros mismos creando un proceso hijo
    pid = fork();

    // Si pid es < 0, fork() falló
    if (pid < 0) {
        // Mostrar la descripción del error y terminar
        std::cerr << strerror(errno) << std::endl;
        exit(10);
    }

    // Si pid es > 0, estamos en el proceso padre
    if (pid > 0) {
        // Terminar el proceso para demonizar el hijo y que este quede como hijo de init
        exit(0);
    }
    // Proceso hijo demonizado en este caso se instacia solamente el objeto con un bucle de mensajes propio del objeto
    Servidor Servidord;
    umask(0);
    // Cambiar el usuario efectivo del proceso
    /*passwd* user = getpwnam("Visord");
    seteuid(user->pw_uid);

    // Cambiar el grupo efectivo del proceso
    group* group = getgrnam("Visord");
    setegid(group->gr_gid);
*/
    openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "Demonio iniciado con éxito\n");
    // Intentar crear una nueva sesión
    sid = setsid();
    if (sid < 0) {
        syslog(LOG_ERR, "No fue posible crear una nueva sesión\n");
        exit(11);
        // Error 11 si no se pudo cambiar de sesíon al proceso hijo
    }
    // Cambiar el directorio de trabajo de proceso
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "No fue posible cambiar el directorio de "
                       "trabajo a /\n");
        exit(12);
    }

    // Cerrar los descriptores de la E/S estándar y reabrirlos para volcar la información de los sockets a el fichero vacío
    close(STDIN_FILENO);                    // fd 0
    close(STDOUT_FILENO);                   // fd 1
    close(STDERR_FILENO);                   // fd 2

    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2
    QFile visordpid("/var/run/Visord.pid");
    visordpid.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream texto(&visordpid);
    texto << getpid() << endl;
    visordpid.close();

    visordpid.close();
    return a.exec();
    closelog();

}
