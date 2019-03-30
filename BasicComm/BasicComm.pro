#-------------------------------------------------
#
# Project created by QtCreator 2019-03-29T20:29:37
#
#-------------------------------------------------
include(./BasicComm.pri)
BC_VER = 0.05
QT       += sql

TARGET = BasicComm
TEMPLATE = lib

DEFINES += BASICCOMM_LIBRARY _BC_NO_TRACE_DLL_ATTACH_DETACH
DESTDIR = $$PWD/lib/

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug, debug|release) {
    TARGET = BasicCommd
} else {
    TARGET = BasicComm
}



SOURCES += src/main.cpp \
    src/IxProtocol/AbstractProtocol.cpp \
    src/CommManager.cpp \
    src/entity/CommandObject.cpp \
    src/entity/DataObject.cpp \
    src/IxComm/AbstractComm.cpp \
    src/IxComm/commimpl/ComFullDuplex.cpp \
    src/IxComm/commimpl/CommNetwork.cpp \
    src/IxComm/commimpl/ComHalfDuplex.cpp

HEADERS += ./include/bc_global.h \
    include/IxProtocol/AbstractProtocol.h \
    ./include/BCPrecompiled.h \
    include/CommManager.h \
    include/entity/CommandObject.h \
    include/entity/DataObject.h \
    include/IxComm/AbstractComm.h \
    include/IxComm/commimpl/ComFullDuplex.h \
    include/IxComm/commimpl/CommNetwork.h \
    include/IxComm/commimpl/ComHalfDuplex.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
