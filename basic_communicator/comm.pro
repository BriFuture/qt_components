#-------------------------------------------------
#
# Project created by QtCreator 2018-07-18T11:26:50
#
#-------------------------------------------------

QT -= gui
QT += core

TARGET = comm
TEMPLATE = lib
CONFIG += staticlib

DEFINES += COMM_LIBRARY

TRANSLATIONS += my_comm_zh_CN.ts\
                my_comm_en_US.ts

include(communicator.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

