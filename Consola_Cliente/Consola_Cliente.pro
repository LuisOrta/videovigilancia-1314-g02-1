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
