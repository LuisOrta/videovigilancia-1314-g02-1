#ifndef SELCAMARA_H
#define SELCAMARA_H

#include <QDialog>

namespace Ui {
class SelCamara;
}

class SelCamara : public QDialog
{
    Q_OBJECT

public:
    explicit SelCamara(QWidget *parent = 0);
    ~SelCamara();
    QList<QByteArray> devices;

private slots:
    void on_BotonSel_clicked();

private:
    Ui::SelCamara *ui;
};

#endif // SELCAMARA_H
