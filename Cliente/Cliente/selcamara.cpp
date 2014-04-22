#include "selcamara.h"
#include "ui_selcamara.h"
#include <QSettings>
#include <QCamera>
#include "QByteArray"

SelCamara::SelCamara(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelCamara)
{
    ui->setupUi(this);
    QSettings sett;

    devices = QCamera::availableDevices();

    for(int i=0; i<devices.length();i++){
        ui->comboBox->addItem(devices[i]);
    }

}


SelCamara::~SelCamara()
{
    delete ui;
}

void SelCamara::on_BotonSel_clicked()
{
    QSettings sett;
    sett.setValue("Camera", ui->comboBox->currentText());
    this->close();
}
