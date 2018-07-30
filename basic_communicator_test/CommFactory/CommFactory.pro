#-------------------------------------------------
#
# Project created by QtCreator 2018-07-18T12:10:58
#
#-------------------------------------------------

QT       += testlib serialport bluetooth network

QT       -= gui

TARGET = tst_commfactorytest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_commfactorytest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../basic_communicator/communicator.pri)

INCLUDEPATH += $$PWD/../../basic_communicator
