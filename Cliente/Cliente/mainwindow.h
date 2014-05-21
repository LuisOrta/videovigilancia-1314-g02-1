#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMovie"
#include "QCamera"
#include "QCameraViewfinder"
#include "capturebuffer.h"
#include "QTcpSocket"
#include "opencv2/opencv.hpp"
#include "cvmatandqimage.h"
using namespace cv;

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
    int n_camara;
    cv::BackgroundSubtractorMOG2 BGSubstractor;
    bool enviar;
    typedef std::vector<cv::Rect> vector_rectangulos;
    vector_rectangulos r;


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbrir_triggered();

    void on_actionAcerca_de_triggered();

    void on_checkBox_clicked();

    void showFrame(const QImage &rect);

    void on_actionCapturar_desde_WebCam_triggered();

    void on_actionSeleccionar_Camara_triggered();

    void on_actionCambiar_IP_Puerto_triggered();

    void on_actionParar_Captura_triggered();

    void manipuladorError(QAbstractSocket::SocketError socketError);

    void socketDesconectado();

    QImage detect_mov(cv::Mat);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
