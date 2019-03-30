#ifndef AbstractComm_H
#define AbstractComm_H

#ifdef _MSC_VER
#pragma once
#endif

#include <QObject>
#include <QIODevice>
#include <QDebug>
#include "bc_global.h"

class BC_DLL_EXPORT AbstractComm : public QObject {
    Q_OBJECT

public:
    static const QByteArray LineSeparator;
    static const int PacketMaxSize;

    // 是否能在半双工模式下工作
    bool isHalfDuplex();

    bool isOpened() const;
    void setOpened(bool opened);

    QString deviceName() const;

    QString errorString() const;

    QString type() const;

    QString desc() const;

    QString mode() const;

private:
    // 是否打开
    bool m_opened = false;
    // 对应的设备名称
    QString m_deviceName;
    QString m_errorString;
    // 表明类型：串口，socket 或者 蓝牙
    QString m_type;
    // 该通讯器的描述信息
    QString m_desc;
    // 用于表明该通讯器的模式
    QString m_mode;
};

#endif // AbstractComm_H
