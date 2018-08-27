#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include <QObject>
#include "dataobject.h"
#include "commandobject.h"

class CommandObject;
class DataObject;

class AbstractProtocol : public QObject
{
    Q_OBJECT
public:
    static const int defaultMaxExecuteTimes;
    static const int defaultTimeout;
    static QByteArray xorCODE(const QByteArray &ba);


    explicit AbstractProtocol(QObject *parent = 0);

    virtual const int remainCmdCount() = 0;
    virtual const int totalCmdCount()  = 0;
    virtual void clearCmdCount() = 0;

    virtual CommandObject *getFirstCmd() = 0;

    bool enabled() const;
    void setEnabled(bool enabled);

    int priority() const;
    void setPriority(int priority);

    int getCmdExecuteTime() const;
    void setCmdExecuteTime(int cmdExecuteTime);

    int getCmdTimeout() const;
    void setCmdTimeout(int cmdTimeout);


signals:
    void cmdAdded();
    void skipCurrentQuery();

public slots:
    virtual void lastQueryFailed() = 0;
    virtual void stopRemainCmd() = 0;
    virtual bool processData(DataObject *dataObj) = 0;

protected:
    bool m_enabled;
    int  m_priority;
    int  m_cmdExecuteTime;
    int  m_cmdTimeout;
};

#endif // ABSTRACTPROTOCOL_H
