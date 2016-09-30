#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T20:53:26
#
#-------------------------------------------------

QT       += core gui network

DEFINES += __print="\"qDebug()<< __FILE__ << __LINE__ <<  Q_FUNC_INFO\""

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UDPClient
TEMPLATE = app


SOURCES += main.cpp\
        user.cpp

HEADERS  += user.h

FORMS    += user.ui
