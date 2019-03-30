#ifndef COMMANDOBJECT_H
#define COMMANDOBJECT_H

#ifdef _MSC_VER
#pragma once
#endif

#include <QObject>
#include <bc_global.h>

class BC_DLL_EXPORT CommandObject : public QObject
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

    explicit CommandObject(QObject *parent = 0);

    QByteArray &contents();
    void setContents(const QByteArray &value);

    State state() const;
    void setState(const State &value);

    int execCount() const;
    void execute();
    void clearExecCount();

    int index() const;
    void setIndex(int index);

signals:
    void stateChanged(const State &m_state);

public slots:

private:
    QByteArray m_contents;
    State m_state;
    int m_index = 0;
    int m_execCount = 0;
};

#endif // COMMANDOBJECT_H
