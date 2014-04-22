#ifndef CAPTUREBUFFER_H
#define CAPTUREBUFFER_H

#include <QAbstractVideoSurface>
#include <QAbstractVideoBuffer>
#include "QRect"

class capturebuffer : public QAbstractVideoSurface
{
     Q_OBJECT
signals:
    void mostrar(QImage);

public:

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType =
            QAbstractVideoBuffer::NoHandle) const
    {
        // A través de este método nos preguntan que formatos de
        // vídeo soportamos. Como vamos a guardar los frames en
        // objetos QImage nos sirve cualquiera de los formatos
        // sorportados por dicha clase: http://kcy.me/z6pa
        QList<QVideoFrame::PixelFormat> formats;
        formats << QVideoFrame::Format_ARGB32;
        formats << QVideoFrame::Format_ARGB32_Premultiplied;
        formats << QVideoFrame::Format_RGB32;
        formats << QVideoFrame::Format_RGB24;
        formats << QVideoFrame::Format_RGB565;
        formats << QVideoFrame::Format_RGB555;
        formats << QVideoFrame::Format_CameraRaw;
        return formats;
    }

    bool present(const QVideoFrame &frame)
    {
        // A través de este método nos darán el frame para que lo mostremos.
        QVideoFrame frame2 = frame;
        frame2.map(QAbstractVideoBuffer::ReadOnly);

        QImage frameAsImage = QImage(frame2.bits(), frame2.width(),
            frame2.height(), frame2.bytesPerLine(),
            QVideoFrame::imageFormatFromPixelFormat(frame2.pixelFormat()));

        // Aquí el código que manipula frameAsImage...

        //Mostrar imagen
        emit mostrar(frameAsImage);

        frame2.unmap();
        return true;
    }
};

#endif // CAPTUREBUFFER_H
