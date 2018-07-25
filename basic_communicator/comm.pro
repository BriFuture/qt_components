#-------------------------------------------------
#
# Project created by QtCreator 2018-07-18T11:26:50
#
#-------------------------------------------------

QT       -= gui
QT += core serialport bluetooth network

TARGET = comm
TEMPLATE = lib
CONFIG += staticlib

DEFINES += COMM_LIBRARY

TRANSLATIONS += my_comm_zh_CN.ts\
                my_comm_en_US.ts

SOURCES += abstractcomm.cpp \
    comfullduplex.cpp \
    comhalfduplex.cpp \
    commbluetooth.cpp \
    commnetwork.cpp \
    virtualcom.cpp \
    commfactory.cpp \
    commglobal.cpp \
    commmanager.cpp \
    abstractprotocol.cpp

HEADERS += abstractcomm.h \
    comfullduplex.h \
    comhalfduplex.h \
    commbluetooth.h \
    commnetwork.h \
    virtualcom.h \
    commfactory.h \
    commglobal.h \
    commmanager.h \
    abstractprotocol.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

#-----------------------------------
# @author BriFuture
# @details this is used for automated build
#
# Copies the given files to the destination directory
#-----------------------------------

## set destdir
DESTDIR = $$OUT_PWD
defineTest(resToDestdir) {
    files = $$1
    ddir  = $$2

    for(FILE, files) {
        # Replace slashes in paths with backslashes for Windows
        DDIR = $$DESTDIR/$$ddir/
        ## if res folder is created, then no need to exec mkdir
        ## note, whatever the platform you are using( Windows, Linux, Mac),
        ## the directory seperator must be slash "/"
        !exists($$DDIR) {
            log($$DDIR not  exists)
            mkpath($$DDIR)
        }
        win32:DDIR ~= s,/,\\,g
        win32:FILE ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
#        QMAKE_POST_LINK += echo $$FILE $$escape_expand(\\n\\t)
#        message(FILE $$FILE)
    }

#    QMAKE_POST_LINK += echo $$quote($$PWD  == $$DESTDIR)
#    message(LINK: $$QMAKE_POST_LINK)
    export(QMAKE_POST_LINK)
    return(true)
}

HEADER_FILES = $$PWD/*.h
#resToDestdir($$HEADER_FILES)

