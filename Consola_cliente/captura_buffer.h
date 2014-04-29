#ifndef CAPTURA_BUFFER_H
#define CAPTURA_BUFFER_H
#include <QAbstractVideoSurface>
class Captura_Buffer : public QAbstractVideoSurface
{
   Q_OBJECT

signals:
   void mostrar_marco(QImage imagen);
public:
    Captura_Buffer(QObject * parent = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                   QAbstractVideoBuffer::HandleType) const;
     bool present(const QVideoFrame &frame);
};

#endif // CAPTURA_BUFFER_H
