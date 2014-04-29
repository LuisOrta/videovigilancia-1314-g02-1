#-------------------------------------------------
#
# Project created by QtCreator 2014-04-19T20:19:19
#
#-------------------------------------------------

QT       += core
QT       += multimedia multimediawidgets
QT       -= gui


TARGET = consola_cliente
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Consola.cpp \
    Manejador.cpp \
    captura_buffer.cpp \
    Captura.cpp

HEADERS += \
    Consola.h \
    Manejador.h \
    captura_buffer.h \
    Captura.h
