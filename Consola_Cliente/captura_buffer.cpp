#include "captura_buffer.h"

Captura_Buffer::Captura_Buffer(QObject * parent):QAbstractVideoSurface(parent){}
QList<QVideoFrame::PixelFormat>Captura_Buffer::supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType ) const
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
    return formats;

}
bool Captura_Buffer::present(const QVideoFrame &frame)
{

    // A través de este método nos darán el frame para que
    // lo mostremos.
    QVideoFrame a = frame;
    a.map(QAbstractVideoBuffer::ReadOnly);
    QImage frameAsImage = QImage(frame.bits(), frame.width(),
    frame.height(), frame.bytesPerLine(),
    QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));
    if(!frameAsImage.isNull())
        emit mostrar_marco(frameAsImage);
    a.unmap();

    return true;
}




