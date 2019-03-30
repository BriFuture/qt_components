#ifndef AbstractComm_H
#define AbstractComm_H

#ifdef _MSC_VER
#pragma once
#endif

#include <QObject>
#include <QIODevice>
#include <QDebug>

#include <bc_global.h>

class BC_DLL_EXPORT AbstractComm : public QObject {
    Q_OBJECT

public:
    static const QByteArray LineSeparator;
    static const int PacketMaxSize;

    // 是否能在半双工模式下工作
    bool isHalfDuplex();

    bool isOpened() const;

    bool started() const;
    void setStarted(bool started);

    QString deviceName() const;

    QString errorString() const;

    QString type() const;

    QString desc() const;

    QString mode() const;

signals:
    void rawDataRecved(const QByteArray &data);
    void lineDataRecved(const QByteArray &data);
    void dataSend(const QByteArray &data);
    // 状态发生改变
    void stateChanged();
    void mayMess();

public slots:
    virtual void init()  = 0;

    virtual void close() = 0;

    virtual void open(const QString &name = QString()) = 0;

    virtual void setCommProperty(const QByteArray &key, const QVariant &value) = 0;

    virtual void write(const QByteArray &data)    = 0;// = 0 标记这是一个纯虚方法，表示该方法必须被子类重写

protected slots:
    void close(QIODevice *device);

    void writeData(QIODevice *device, const QByteArray &rawdata);

protected:
    void splitData(QByteArray &data);

    // 是否打开
    bool m_opened = false;
    bool m_started = false;
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
