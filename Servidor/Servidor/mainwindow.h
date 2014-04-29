#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMovie"
#include "QCamera"
#include "QCameraViewfinder"
#include "capturebuffer.h"
#include "QTcpServer"
#include "QTcpSocket"

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
    QTcpSocket *clientConnection;
    int buffsize;
    int protocol_version;
    int corrupt_counter;
    QByteArray readbuffer;
    QImage imagebuff;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbrir_triggered();

    void on_actionAcerca_de_triggered();

    void on_checkBox_clicked();

    void showFrame(const QImage &rect);

    void escucha();

    void on_actionSeleccionar_Camara_triggered();

    void leerDatos();

    void on_actionSeleccionar_puerto_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H