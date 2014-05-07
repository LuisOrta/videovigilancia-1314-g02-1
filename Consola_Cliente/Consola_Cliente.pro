#-------------------------------------------------
#
# Project created by QtCreator 2014-05-06T13:02:50
#
#-------------------------------------------------

QT       += core
QT       +=multimedia multimediawidgets
QT       -= gui

TARGET = Consola_Cliente
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    captura_buffer.cpp \
    Captura.cpp \
    Consola.cpp \
    Manejador.cpp

HEADERS += \
    Captura_buffer.h \
    captura_buffer.h \
    Captura.h \
    Consola.h \
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
    DEFINES += APP_CONFFILE=\\\"$$CONFDIR/ficheroConsolaCliente.ini\\\"
    # Install
    #
    INSTALLS += target config

    ## Instalar ejecutable
    target.path = $$BINDIR

    ## Instalar archivo de configuración
    config.path = $$CONFDIR
    config.files += ficheroConsolaCliente.ini
}

