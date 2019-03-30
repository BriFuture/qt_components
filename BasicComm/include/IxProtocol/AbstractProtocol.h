#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#ifdef _MSC_VER
#pragma once
#endif

#include <QObject>
#include <QVector>
#include <bc_global.h>

#include <entity/CommandObject.h>
#include <entity/DataObject.h>

class BC_DLL_EXPORT AbstractProtocol : public QObject
{
    Q_OBJECT
public:
    static const int DefaultMaxExecuteTimes;
    static const int DefaultWaitTime;
    static const int CommandQueueSize;
    static QByteArray XOR_Code(const QByteArray &ba);

    struct DataProcessor {
        QByteArray header;
        QObject *object;
        const char *method;
    };

    explicit AbstractProtocol(QObject *parent = 0);

    bool enabled() const;
    void setEnabled(bool enabled);

    int priority() const;
    void setPriority(int priority);

    int cmdMaxExecCount() const;
    void setCmdMaxExecCount(int maxExecCount);

    int cmdWaitTime() const;
    void setCmdWaitTime(int millseconds);

    void addProcessor(const QByteArray &header, QObject *object, const char *method);
    void removeProcessor(const QByteArray &header, QObject *object, const char *method);

    virtual CommandObject *getFirstCmd();
    virtual void addCommand(const QByteArray &content);
    virtual const int remainCmdCount();
public slots:
    virtual void lastQueryFailed();
    virtual void stopRemainCmd();
    virtual bool processData(const DataObject &dataObj);

signals:
    void cmdAdded();
    void currentQuerySkipped();

protected:
    bool dispatch(const DataObject &data);

protected:
    bool m_enabled;
    int  m_priority;
    int  m_cmdMaxExecCount = DefaultMaxExecuteTimes;
    int  m_cmdWaitTime = DefaultWaitTime;

    QVector<DataProcessor> m_processors;
    QVector<CommandObject *> m_cmdQueue;

    int m_cmdQueueHeader = 0;
    int m_cmdQueueTail = 0;
    int m_cmdQueueSize = 0;
};

Q_DECLARE_METATYPE(AbstractProtocol::DataProcessor)

#endif // ABSTRACTPROTOCOL_H
