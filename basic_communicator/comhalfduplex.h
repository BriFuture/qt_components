#ifndef COMHALFDUPLEX_H
#define COMHALFDUPLEX_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QMutex>

#include "abstractcomm.h"

class ComHalfDuplex : public AbstractComm
{
    Q_OBJECT

public:
//    static const CommInfo commInfo;

    Q_INVOKABLE ComHalfDuplex();
    ~ComHalfDuplex();

    int blockTime() const;
    void setBlockTime(int blockTime);

signals:

public slots:
    void init() Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;
    void openComm(const QString &name) Q_DECL_OVERRIDE;
    void setCommProperty(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    void write(const QByteArray &cmd) Q_DECL_OVERRIDE;

private slots:
    void onRead() Q_DECL_OVERRIDE;

private:
    void handleQuery();
    QMutex *mutex = Q_NULLPTR;
    int m_blockTime = 3;  // 3 ms

    QByteArray m_cmd;
    QSerialPort *m_device;
};

#endif // COMHALFDUPLEX_H
