#-------------------------------------------------
#
# Project created by QtCreator 2018-07-20T12:09:33
#
#-------------------------------------------------

QT       += testlib serialport bluetooth network

QT       -= gui

TARGET = tst_abstractcommtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_abstractcommtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../basic_communicator/communicator.pri)

INCLUDEPATH += $$PWD/../../basic_communicator
