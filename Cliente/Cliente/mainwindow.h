#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMovie"
#include "QCamera"
#include "QCameraViewfinder"
#include "capturebuffer.h"
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
    QTcpSocket *socket;
    int cierre;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbrir_triggered();

    void on_actionAcerca_de_triggered();

    void on_checkBox_clicked(bool checked);

    void on_checkBox_clicked();

    void on_Pause_clicked();

    void showFrame(const QImage &rect);

    void on_actionCapturar_desde_WebCam_triggered();

    void on_actionSeleccionar_Camara_triggered();

    void on_actionCambiar_IP_Puerto_triggered();

    void on_actionParar_Captura_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
