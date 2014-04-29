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

unix{          # Esta configuración específica de Linux y UNIX
    # Variables
    #
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    MOC_DIR = ./moc
    OBJECTS_DIR = ./obj
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc/xdg

    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$${TARGET}
    }
    DEFINES += APP_DATADIR=\\\"$$DATADIR\\\"
    DEFINES += APP_VARDIR=\\\"$$VARDIR\\\"
    DEFINES += APP_CONFFILE=\\\"$$CONFDIR/ficheroServer.ini\\\"
    # Install
    #
    INSTALLS += target config vardir

    ## Instalar ejecutable
    target.path = $$BINDIR

    ## Instalar archivo de configuración
    config.path = $$CONFDIR
    config.files += ficheroServer.ini

    ## Crear directorio de archivos variables
    vardir.path = $$VARDIR
    vardir.commands = :
}

