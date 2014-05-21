#-------------------------------------------------
#
# Project created by QtCreator 2014-05-18T20:07:34
#
#-------------------------------------------------

QT       += core
QT       += multimedia multimediawidgets
QT       += sql
QT       -= gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Visord
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    servidor.cpp \
    Manejador.cpp

HEADERS += \
    servidor.h \
    Manejador.h

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

OTHER_FILES +=
