#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H

#include <QObject>

class AbstractProtocol : public QObject
{
    Q_OBJECT
public:
    explicit AbstractProtocol(QObject *parent = 0);
    static QByteArray xorCODE(const QByteArray &ba);

    virtual const int remainCmdCount() = 0;
    virtual const QByteArray firstQueryContent() = 0;
    virtual const int totalCmdCount()  = 0;
    virtual void clearRemainCmd() = 0;
    virtual void startTiming() = 0;

    bool enabled() const;
    void setEnabled(bool enabled);

    int priority() const;
    void setPriority(int priority);

signals:
    void skipCurrentQuery();

public slots:
    virtual void stopQuery() = 0;
    virtual bool processData(const QByteArray &data, const int index=0) = 0;

protected:
    bool m_enabled;
    int  m_priority;

};

#endif // ABSTRACTPROTOCOL_H
