#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include <QList>
#include <QThread>
#include <QTimer>
#include <QVector>

#include <bc_global.h>
#include <IxComm/AbstractComm.h>
#include <IxProtocol/AbstractProtocol.h>

class BC_DLL_EXPORT CommManager : public QObject
{
    Q_OBJECT
public:
    explicit CommManager(QObject *parent = 0);
    virtual ~CommManager();

    // comm 相关
    AbstractComm *currentComm() const;
    AbstractComm *getComm(const QString &mode);
    void open(const QString &name, const QString &mode = QString());
    void addComm(AbstractComm *ac);
    void setCommMode(const QString &mode);

    // protocol query
    void startQuery();
    void stopCurrQuery();
    void stopAllQuery();

    // protocol 相关
    void addProtocol(AbstractProtocol *p);
    void delProtocol(AbstractProtocol *p);
    void removeAllProtocol();
    const QVector<AbstractProtocol *> &protocolList();
signals:
    void dataRecved(const DataObject &dataObj);
    void cmdCountChanged(int remain, int total);
    void setCommProperty(const QByteArray &key, const QVariant &value);
    void commChanged(AbstractComm *comm);

protected slots:
    void onRecvLineData(const QByteArray &data);

protected:
    // protocol query
    void nextQuery();

    void calcProtocolCmd();

    bool execCmd();

    AbstractComm *createComm(const QString &mode);

private:
    QThread           *m_subThread = Q_NULLPTR;
    AbstractComm      *m_currComm = Q_NULLPTR;
    CommandObject     *m_currCmd = 0;
    AbstractProtocol  *m_currProtocol = 0;
    QVector<AbstractComm *> m_availableComms;
    QVector<AbstractProtocol *> m_protocols;

    bool m_querying = false;
    int remainCmdCount = 0;
    int totalCmdCount  = 0;
};

#endif // COMMANAGER_H
