#-------------------------------------------------
#
# Project created by QtCreator 2018-07-18T11:26:50
#
#-------------------------------------------------

QT       -= gui
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

SOURCES += abstractcomm.cpp \
    comfullduplex.cpp \
    comhalfduplex.cpp \
    commbluetooth.cpp \
    commnetwork.cpp \
    virtualcom.cpp \
    commfactory.cpp \
    commglobal.cpp \
    commmanager.cpp \
    abstractprotocol.cpp \
    commandobject.cpp

HEADERS += abstractcomm.h \
    comfullduplex.h \
    comhalfduplex.h \
    commbluetooth.h \
    commnetwork.h \
    virtualcom.h \
    commfactory.h \
    commglobal.h \
    commmanager.h \
    abstractprotocol.h \
    commandobject.h
