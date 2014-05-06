/********************************************************************************
** Form generated from reading UI file 'acercade.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACERCADE_H
#define UI_ACERCADE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AcercaDe
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QDialog *AcercaDe)
    {
        if (AcercaDe->objectName().isEmpty())
            AcercaDe->setObjectName(QStringLiteral("AcercaDe"));
        AcercaDe->resize(400, 300);
        verticalLayout = new QVBoxLayout(AcercaDe);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(AcercaDe);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        pushButton = new QPushButton(AcercaDe);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);


        retranslateUi(AcercaDe);

        QMetaObject::connectSlotsByName(AcercaDe);
    } // setupUi

    void retranslateUi(QDialog *AcercaDe)
    {
        AcercaDe->setWindowTitle(QApplication::translate("AcercaDe", "Dialog", 0));
        label->setText(QApplication::translate("AcercaDe", "TextLabel", 0));
        pushButton->setText(QApplication::translate("AcercaDe", "Aceptar", 0));
    } // retranslateUi

};

namespace Ui {
    class AcercaDe: public Ui_AcercaDe {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACERCADE_H
