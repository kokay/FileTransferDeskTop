#-------------------------------------------------
#
# Project created by QtCreator 2017-02-02T21:32:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileTransferDesktop
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp\
        FileInfo.cpp\
        FileTransferClient.cpp

HEADERS  += dialog.h\
        FileInfo.h\
        FileTransferClient.h

FORMS    += dialog.ui

CONFIG += c++11

LIBS += -lboost_system \
        -lboost_filesystem
