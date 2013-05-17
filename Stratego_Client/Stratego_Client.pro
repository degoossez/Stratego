#-------------------------------------------------
#
# Project created by QtCreator 2013-04-26T12:54:44
#
#-------------------------------------------------
QT       += network
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Stratego_Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connection.cpp \
    mygraphicsscene.cpp

HEADERS  += mainwindow.h \
    connection.h \
    mygraphicsscene.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
