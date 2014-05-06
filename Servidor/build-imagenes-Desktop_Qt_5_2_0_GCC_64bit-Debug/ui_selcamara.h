/********************************************************************************
** Form generated from reading UI file 'selcamara.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELCAMARA_H
#define UI_SELCAMARA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SelCamara
{
public:
    QComboBox *comboBox;
    QPushButton *BotonSel;

    void setupUi(QDialog *SelCamara)
    {
        if (SelCamara->objectName().isEmpty())
            SelCamara->setObjectName(QStringLiteral("SelCamara"));
        SelCamara->resize(508, 167);
        comboBox = new QComboBox(SelCamara);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(90, 60, 331, 23));
        BotonSel = new QPushButton(SelCamara);
        BotonSel->setObjectName(QStringLiteral("BotonSel"));
        BotonSel->setGeometry(QRect(210, 100, 80, 23));

        retranslateUi(SelCamara);

        QMetaObject::connectSlotsByName(SelCamara);
    } // setupUi

    void retranslateUi(QDialog *SelCamara)
    {
        SelCamara->setWindowTitle(QApplication::translate("SelCamara", "Dialog", 0));
        BotonSel->setText(QApplication::translate("SelCamara", "Seleccionar", 0));
    } // retranslateUi

};

namespace Ui {
    class SelCamara: public Ui_SelCamara {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELCAMARA_H
