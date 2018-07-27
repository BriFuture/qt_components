#ifndef COMFULLDUPLEX_H
#define COMFULLDUPLEX_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QRegExp>
#include <QByteArrayList>
#include <QHash>
#include <QMutex>

#include "abstractcomm.h"

class ComFullDuplex : public AbstractComm
{
    Q_OBJECT

public: 
//    static const CommInfo commInfo;

    Q_INVOKABLE ComFullDuplex();
    ~ComFullDuplex();

signals:

public slots:
    void init() Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;
    void openComm(const QString &name) Q_DECL_OVERRIDE;

    void setCommProperty(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    void write(const QByteArray &data) Q_DECL_OVERRIDE;

private slots:
    void onRead() Q_DECL_OVERRIDE;

private:
    QSerialPort *m_device;
};


#endif // COMFULLDUPLEX_H
