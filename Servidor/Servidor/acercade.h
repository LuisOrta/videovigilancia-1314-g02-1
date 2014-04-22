#ifndef ACERCADE_H
#define ACERCADE_H

#include <QDialog>

namespace Ui {
class AcercaDe;
}

class AcercaDe : public QDialog
{
    Q_OBJECT

public:
    explicit AcercaDe(QWidget *parent = 0);
    ~AcercaDe();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AcercaDe *ui;
};

#endif // ACERCADE_H
