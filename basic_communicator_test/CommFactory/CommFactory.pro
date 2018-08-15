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
DEFINES += SRCDIR=\\\"$$PWD/\\\" COMMUNICATOR_INCLUDE
include(../../basic_communicator/communicator.pri)
