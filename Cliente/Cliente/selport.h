#ifndef SELPORT_H
#define SELPORT_H

#include <QDialog>

namespace Ui {
class selport;
}

class selport : public QDialog
{
    Q_OBJECT

public:
    explicit selport(QWidget *parent = 0);
    ~selport();

private slots:


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::selport *ui;
};

#endif // SELPORT_H
