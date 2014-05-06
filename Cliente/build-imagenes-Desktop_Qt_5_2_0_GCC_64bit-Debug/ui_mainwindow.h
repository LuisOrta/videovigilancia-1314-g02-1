/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbrir;
    QAction *actionAcerca_de;
    QAction *actionCapturar_desde_WebCam;
    QAction *actionSeleccionar_Camara;
    QAction *actionCambiar_IP_Puerto;
    QAction *actionParar_Captura;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QPushButton *Stop;
    QPushButton *Play;
    QCheckBox *checkBox;
    QLabel *label;
    QMenuBar *menuBar;
    QMenu *menuArchivo;
    QMenu *menuAyuda;
    QMenu *menuPreferencias;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(694, 414);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionAbrir = new QAction(MainWindow);
        actionAbrir->setObjectName(QStringLiteral("actionAbrir"));
        actionAcerca_de = new QAction(MainWindow);
        actionAcerca_de->setObjectName(QStringLiteral("actionAcerca_de"));
        actionCapturar_desde_WebCam = new QAction(MainWindow);
        actionCapturar_desde_WebCam->setObjectName(QStringLiteral("actionCapturar_desde_WebCam"));
        actionSeleccionar_Camara = new QAction(MainWindow);
        actionSeleccionar_Camara->setObjectName(QStringLiteral("actionSeleccionar_Camara"));
        actionCambiar_IP_Puerto = new QAction(MainWindow);
        actionCambiar_IP_Puerto->setObjectName(QStringLiteral("actionCambiar_IP_Puerto"));
        actionParar_Captura = new QAction(MainWindow);
        actionParar_Captura->setObjectName(QStringLiteral("actionParar_Captura"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Stop = new QPushButton(centralWidget);
        Stop->setObjectName(QStringLiteral("Stop"));

        gridLayout->addWidget(Stop, 3, 1, 1, 1);

        Play = new QPushButton(centralWidget);
        Play->setObjectName(QStringLiteral("Play"));

        gridLayout->addWidget(Play, 2, 1, 1, 1);

        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout->addWidget(checkBox, 3, 0, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setScaledContents(true);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 694, 20));
        menuArchivo = new QMenu(menuBar);
        menuArchivo->setObjectName(QStringLiteral("menuArchivo"));
        menuAyuda = new QMenu(menuBar);
        menuAyuda->setObjectName(QStringLiteral("menuAyuda"));
        menuPreferencias = new QMenu(menuBar);
        menuPreferencias->setObjectName(QStringLiteral("menuPreferencias"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuArchivo->menuAction());
        menuBar->addAction(menuAyuda->menuAction());
        menuBar->addAction(menuPreferencias->menuAction());
        menuArchivo->addAction(actionAbrir);
        menuArchivo->addAction(actionCapturar_desde_WebCam);
        menuArchivo->addAction(actionParar_Captura);
        menuAyuda->addAction(actionAcerca_de);
        menuPreferencias->addAction(actionSeleccionar_Camara);
        menuPreferencias->addAction(actionCambiar_IP_Puerto);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionAbrir->setText(QApplication::translate("MainWindow", "Abrir", 0));
        actionAbrir->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0));
        actionAcerca_de->setText(QApplication::translate("MainWindow", "Acerca de...", 0));
        actionCapturar_desde_WebCam->setText(QApplication::translate("MainWindow", "Capturar desde WebCam", 0));
        actionSeleccionar_Camara->setText(QApplication::translate("MainWindow", "Seleccionar Camara", 0));
        actionCambiar_IP_Puerto->setText(QApplication::translate("MainWindow", "Cambiar IP/Puerto", 0));
        actionParar_Captura->setText(QApplication::translate("MainWindow", "Parar Captura", 0));
        Stop->setText(QApplication::translate("MainWindow", "Stop", 0));
        Play->setText(QApplication::translate("MainWindow", "Play", 0));
        checkBox->setText(QApplication::translate("MainWindow", "Autorun", 0));
        label->setText(QString());
        menuArchivo->setTitle(QApplication::translate("MainWindow", "Archivo", 0));
        menuAyuda->setTitle(QApplication::translate("MainWindow", "Ayuda", 0));
        menuPreferencias->setTitle(QApplication::translate("MainWindow", "Preferencias", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
