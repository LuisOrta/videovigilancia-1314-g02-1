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
