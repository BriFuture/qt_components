#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QList>
#include <QThread>
#include <QTimer>

#include "abstract/abstractcomm.h"
#include "bc_global.h"

class AbstractProtocol;
class CommandObject;
#include "commfactory.h"

class CommManagerUtil : public QObject {
    Q_OBJECT
    friend class CommManager;
public:
    void calcProtocolCmd();
    bool executeCmd();

private:
    explicit CommManagerUtil(QObject *parent = 0) : QObject(parent) {}
    QList<AbstractProtocol *> protocolList;

    AbstractProtocol *currProtocol = Q_NULLPTR;
    CommandObject *currCmd;
    int cmdExecuteTimes = 0;
    QTimer timer;

    int lineCount      = 0;
    int remainCmdCount = 0;
    int totalCmdCount  = 0;
    bool querying      = false;
    QString commClassName;
};

class BC_DLL_EXPORT CommManager : public QObject
{
    Q_OBJECT

public:
    explicit CommManager(QObject *parent = 0);
    ~CommManager();

    void resetMode(const QString &type, const bool halfDuplex);
    void resetMode(const QString &className);

    void addProtocol(AbstractProtocol *p);
    void delProtocol(AbstractProtocol *p);
    void removeAllProtocol();

    const QList<AbstractProtocol *> &protocolList();
    AbstractProtocol *protocol(const QString &objectName);

    const CommInfo &currentCommInfo();

    template <typename T>
    T getProtocol(const QString &objectName) {
        AbstractProtocol *p = protocol(objectName);
        return static_cast<T>(p);
    }

    const CommState &state();
    AbstractComm *abstractComm() const;

signals:
    void setCommProperty(const QString &key, const QVariant &value);

    void commInfoChanged(const CommInfo &ci);
    void codeMayMessed();
    void cmdCountChanged(const int remain, const int all);

    void recvRawData(const QByteArray &data);
    void recvLineData(const QByteArray &data, const int count = 0);
    void sendData(const QByteArray &data);
    void stateChanged(const CommState &state);

public slots:
    virtual void startQuery();
    virtual void stopAllQuery();
    virtual void startNextQuery();
    virtual void close();
    virtual void openDevice(const QString &name = "");

protected slots:
    void onStateChanged(const CommState &state);
    void onRecvLineData(const QByteArray &data);
    void onWriteFinish();
    void onTimeout();
    void write();

protected:
    void init(const QString &name);

    QThread           *m_subThread = Q_NULLPTR;
    AbstractComm      *m_abstractComm = Q_NULLPTR;
    CommState m_state;

    CommManagerUtil util;
};

#endif // COMMANAGER_H
