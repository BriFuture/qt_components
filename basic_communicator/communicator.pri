QT += serialport bluetooth network

if(contains(DEFINES, COMMUNICATOR_INCLUDE)) {
    SOURCES += \
        $$PWD/abstractcomm.cpp \
        $$PWD/comfullduplex.cpp \
        $$PWD/comhalfduplex.cpp \
        $$PWD/commbluetooth.cpp \
        $$PWD/commnetwork.cpp \
        $$PWD/virtualcom.cpp \
        $$PWD/commfactory.cpp \
        $$PWD/commglobal.cpp \
        $$PWD/commmanager.cpp \
        $$PWD/abstractprotocol.cpp \
        $$PWD/commandobject.cpp \
        $$PWD/dataobject.cpp

    HEADERS += \
        $$PWD/abstractcomm.h \
        $$PWD/comfullduplex.h \
        $$PWD/comhalfduplex.h \
        $$PWD/commbluetooth.h \
        $$PWD/commnetwork.h \
        $$PWD/virtualcom.h \
        $$PWD/commfactory.h \
        $$PWD/commglobal.h \
        $$PWD/commmanager.h \
        $$PWD/abstractprotocol.h \
        $$PWD/commandobject.h \
        $$PWD/dataobject.h
}

INCLUDEPATH += $$PWD
