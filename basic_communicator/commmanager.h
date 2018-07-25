#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QList>
#include <QThread>
#include <QTimer>

#include "abstractprotocol.h"
#include "abstractcomm.h"
#include "commfactory.h"

class CommManagerUtil : public QObject {
    Q_OBJECT
    friend class CommManager;
public:
    void calcProtocolCmd();

private:
    explicit CommManagerUtil(QObject *parent = 0) : QObject(parent) {}
    QList<AbstractProtocol *> protocolList;
    AbstractProtocol         *currProtocol = Q_NULLPTR;
    int lineCount      = 0;
    int remainCmdCount = 0;
    int totalCmdCount  = 0;
    bool querying      = false;
    QString commClassName;
};

class CommManager : public QObject
{
    Q_OBJECT

public:
    explicit CommManager(QObject *parent = 0);
    ~CommManager();

    void resetMode(const QString &type, const bool halfDuplex);
    void resetMode(const QString &className);

    void addProtocol(AbstractProtocol *p);
    void delProtocol(AbstractProtocol *p);
    const QList<AbstractProtocol *> &protocolList();
    AbstractProtocol *protocol(const QString &objectName);

    const CommInfo &currentCommInfo();

    template <typename T>
    T getProtocol(const QString &objectName) {
        AbstractProtocol *p = protocol(objectName);
        return static_cast<T>(p);
    }

    const CommState &state();

signals:
    void commInfoChanged(const CommInfo &ci);
    void codeMayMessed();
    void cmdCountChanged(const int remain, const int all);

    void recvRawData(const QByteArray &data);
    void recvLineData(const QByteArray &data, const int count = 0);
    void sendData(const QByteArray &data);
    void stateChanged(const CommState &state);

    void setCommProperty(const QString &key, const QVariant &value);

public slots:
    virtual void onQuery();
    virtual void stopAllQuery();
    virtual void stopCurrentQuery();
    virtual void close();

    virtual void openDevice(const QString &name = "");

    void onWriteFinish();

    void onStateChanged(const CommState &state);
    void onRecvLineData(const QByteArray &data);

protected:
    void init(const QString &name);

    QThread           *m_subThread = Q_NULLPTR;
    AbstractComm      *m_abstractComm = Q_NULLPTR;
    CommState m_state;

    CommManagerUtil util;
};

#endif // COMMANAGER_H
