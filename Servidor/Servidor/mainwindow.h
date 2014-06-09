#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMovie"
#include "QCamera"
#include "QCameraViewfinder"
#include "capturebuffer.h"
#include "QTcpServer"
#include "QTcpSocket"
#include "QList"
#include "QtSql/QSqlDatabase"
#include "QSettings"
#include "QDir"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QMovie movie;
    QCamera *cam;
    QList<QByteArray> devices;
    QCameraViewfinder viewfinder;
    capturebuffer *buffer;
    bool isInicio;
    QTcpServer *server;
    //QTcpSocket *clientConnection;
    QList <QTcpSocket *> clientConnections;
    u_int32_t buffsize;
    u_int32_t protocol_version;
    int corrupt_counter;
    int n_camara;
    QByteArray readbuffer;
    QImage imagebuff;
    QSqlDatabase db;
    int cont_rect;
    int contador_;
    QSettings cont_;
    QDir ruta_;
    QString aux;
    struct rect{
      int x;
      int y;
      int w;
      int h;
    };
    std::vector<rect> v_rect;
    u_int32_t n_rec;
    bool terminado_roi;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbrir_triggered();

    void on_actionAcerca_de_triggered();

    void on_checkBox_clicked();

    void showFrame(const QImage &rect);

    void escucha();

    void clienteDesconectado();

    void on_actionSeleccionar_Camara_triggered();

    void leerDatos();

    void on_actionSeleccionar_puerto_triggered();

    void on_actionTerminar_Servidor_triggered();

    void on_actionIniciar_Servidor_triggered();

    void almacenar_metadatos (int nombre,std::vector<rect>);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
