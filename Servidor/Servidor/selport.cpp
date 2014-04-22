#include "selport.h"
#include "ui_selport.h"
#include "QSettings"

selport::selport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selport)
{
    ui->setupUi(this);
}

selport::~selport()
{
    delete ui;
}

void selport::on_pushButton_clicked()
{
    QString port = ui->textEdit->toPlainText();
    QSettings sett;
    sett.setValue("IPPort", port);
    this->close();
}

void selport::on_pushButton_2_clicked()
{
    this->close();
}
