# requires Qt 5.7 and a C++11 compiler
CONFIG += c++11
QT += serialport network bluetooth
contains(DEFINES, _BC_ENABLE_QT_GUI) {
    QT += gui
}

#######################
## Global Parameters ##
#######################

CONFIG += debug_and_release
DEPENDPATH += .
INCLUDEPATH += $$PWD/include \
    $$PWD/include/IxComm \
    $$PWD/include/IxComm\commimpl \
    $$PWD/include/entity \
    $$PWD/include/IxProtocol

MOC_DIR = ./qt/moc
RCC_DIR = ./qt/rcc/src
UI_DIR = ./qt/ui
UI_HEADERS_DIR = ./qt/ui/include
UI_SOURCES_DIR = ./qt/ui/src

CONFIG(debug, debug|release) {
    DEFINES += _BC_MODE_DEBUG
} else {
    DEFINES += _BC_MODE_RELEASE
} # CONFIG(debug, debug|release)
