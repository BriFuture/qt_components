#ifndef AbstractComm_H
#define AbstractComm_H

#include <QObject>
#include <QIODevice>
#include <QDebug>
#include "bc_global.h"

struct BC_DLL_EXPORT CommState {
    bool opened = false;
    QString deviceName;
    QString errorString;
    int baudRate = 9600;
//    QString type;
};

struct BC_DLL_EXPORT CommInfo {
    QString type;
    QString desc;
    bool isHalfDuplex;
};

class BC_DLL_EXPORT OldAbstractComm : public QObject {
    Q_OBJECT

public:
    static const CommInfo commInfo;
    Q_PROPERTY(bool started READ isStarted WRITE setStarted)

    static const QByteArray LineSeparator;
    static const int PacketMaxSize;

    virtual bool isStarted();
    virtual void setStarted(const bool started);

    CommState &state();
    void setState(const CommState &state);

public slots:
    virtual void init()  = 0;
    virtual void close() = 0;
    virtual void openComm(const QString &name) = 0;
    virtual void setCommProperty(const QString &key, const QVariant &value) = 0;
    virtual void write(const QByteArray &data)    = 0;// = 0 标记这是一个纯虚方法，表示该方法必须被子类重写

protected slots:
    virtual void onRead() = 0;

protected:

signals:
    void recvRawData(const QByteArray &data);
    void recvLine(const QByteArray &data);
    void sendData(const QByteArray &data);
    void stateChanged(const CommState &serial);
    void codeMayMessed();

protected:
    explicit OldAbstractComm();
    void close(QIODevice *device);
    Q_INVOKABLE void readDevice(QIODevice *device);
    Q_INVOKABLE void splitData();
    void writeData(QIODevice *device, const QByteArray &rawdata);

    static CommState m_state;

    bool  m_started = false;
    QByteArray m_tmpData;
    QByteArray m_recvData;
};

QDebug BC_DLL_EXPORT operator<<(QDebug debug, const CommInfo &ci);
QDebug BC_DLL_EXPORT operator<<(QDebug debug, const CommState &cs);

Q_DECLARE_METATYPE(CommState)

#endif // AbstractComm_H
