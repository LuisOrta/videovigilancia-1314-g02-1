/********************************************************************************
** Form generated from reading UI file 'selport.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELPORT_H
#define UI_SELPORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_selport
{
public:
    QTextEdit *textEdit;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *selport)
    {
        if (selport->objectName().isEmpty())
            selport->setObjectName(QStringLiteral("selport"));
        selport->resize(400, 300);
        textEdit = new QTextEdit(selport);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(100, 140, 251, 31));
        label = new QLabel(selport);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 150, 57, 15));
        label_2 = new QLabel(selport);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(90, 60, 301, 41));
        pushButton = new QPushButton(selport);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(90, 200, 80, 23));
        pushButton_2 = new QPushButton(selport);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(200, 200, 80, 23));

        retranslateUi(selport);

        QMetaObject::connectSlotsByName(selport);
    } // setupUi

    void retranslateUi(QDialog *selport)
    {
        selport->setWindowTitle(QApplication::translate("selport", "Dialog", 0));
        label->setText(QApplication::translate("selport", "Puerto", 0));
        label_2->setText(QApplication::translate("selport", "El puerto por defecto es 15000", 0));
        pushButton->setText(QApplication::translate("selport", "Aceptar", 0));
        pushButton_2->setText(QApplication::translate("selport", "Salir", 0));
    } // retranslateUi

};

namespace Ui {
    class selport: public Ui_selport {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELPORT_H
