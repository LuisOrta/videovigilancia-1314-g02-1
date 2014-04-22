#include "acercade.h"
#include "ui_acercade.h"

AcercaDe::AcercaDe(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AcercaDe)
{
    ui->setupUi(this);
    ui->label->setText("VISOR DE VIDEOS MJPEG. \n Programa realizado por Luis Antonio Orta Méndez para la asignatura de Sistemas Operativos Avanzados. \n ETSII ULL 2014");

}

AcercaDe::~AcercaDe()
{
    delete ui;
}

void AcercaDe::on_pushButton_clicked()
{
    this-close();
}
