#ifndef CONSOLA_H
#define CONSOLA_H

#include <QObject>
#include <iostream>
#include <QTextStream>
#include <QSettings>
#include <QCamera>
#include <QByteArray>
#include "Captura.h"

class Consola : public QObject
{
    Q_OBJECT

public:
    explicit Consola(Captura *Objeto_Capturador =NULL,QObject *parent = 0);
    ~Consola();

public slots:

    void mostrar_menu(void);

signals:

    void empezar_captura (const QString &ip,const QString &puerto,const QString &dispositivo_sel = NULL);

private:

    QTextStream *qtin_;
    QTextStream *qtout_;
    Captura *objeto_capturador_;

    QString eleccion_;
    QString dispositivo_sel_;
    QString ip_;
    QString puerto_;

    QSettings conf_;
    QList<QByteArray> dispositivos_;




    void mostrar_menu_capturar(void);
    void mostrar_menu_configuracion(void);
    void mostrar_menu_configuracion_ip(void);
    void mostrar_menu_configuracion_puerto(void);
    void mostrar_menu_configuracion_dispositivo_video(void);
};
#endif // CONSOLA_H
