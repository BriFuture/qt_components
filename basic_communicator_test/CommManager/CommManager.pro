#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T19:45:46
#
#-------------------------------------------------

QT       += testlib serialport bluetooth network

QT       -= gui

TARGET = tst_commmanagertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_commmanagertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\" COMMUNICATOR_INCLUDE
include(../../basic_communicator/communicator.pri)
