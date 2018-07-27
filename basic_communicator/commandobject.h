#ifndef COMMANDOBJECT_H
#define COMMANDOBJECT_H

#include <QObject>
#include "abstractprotocol.h"

class AbstractProtocol;

class CommandObject : public QObject
{
    Q_OBJECT
public:
    enum State {
        Waiting   = 0x0,  // never executed
        Executing = 0x1,
        Pending = 0x2,
        Timeout = 0x3, // wait a long time without reply, means FAILURE
        RightReplied = 0x4,
        WrongReplied = 0x5
    };

    explicit CommandObject(AbstractProtocol *parent = 0);
    AbstractProtocol *protocolBelongs();

    int getMaxExecuteTimes() const;
    void setMaxExecuteTimes(int value);

    int getTimeout() const;
    void setTimeout(int value);

    QByteArray contents() const;
    void setContents(const QByteArray &value);

    State state() const;
    void setState(const State &value);

    QByteArray reply() const;
    void setReply(const QByteArray &value);

signals:
    void stateChanged(const State &m_state);

public slots:

private:
    QByteArray m_contents;
    QByteArray m_reply;
    State m_state;
    int maxExecuteTimes;
    int timeout;
};

#endif // COMMANDOBJECT_H
