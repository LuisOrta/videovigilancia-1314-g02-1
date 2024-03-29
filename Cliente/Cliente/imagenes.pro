#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T08:05:59
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imagenes
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    acercade.cpp \
    selcamara.cpp \
    selport.cpp

HEADERS  += mainwindow.h \
    acercade.h \
    capturebuffer.h \
    selcamara.h \
    selport.h

FORMS    += mainwindow.ui \
    acercade.ui \
    selcamara.ui \
    selport.ui

unix {          # Esta configuración específica de Linux y UNIX

    MOC_DIR = ./moc
    OBJECTS_DIR = ./obj
    # Variables
    #
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc/xdg
    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$${TARGET}
    }

    DEFINES += APP_DATADIR=\\\"$$DATADIR\\\"
    DEFINES += APP_VARDIR=\\\"$$VARDIR\\\"
    DEFINES += APP_CONFFILE=\\\"$$CONFDIR/ficheroCliente.ini\\\"

    # Install
    #
    INSTALLS += target config

    ## Instalar ejecutable
    target.path = $$BINDIR

    ## Instalar archivo de configuración
    config.path = $$CONFDIR
    config.files += ficheroCliente.ini
}

include(QtOpenCV.pri)
add_opencv_modules(core video imgproc)
