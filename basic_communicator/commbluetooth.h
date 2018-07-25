#ifndef COMMBLUETOOTH_H
#define COMMBLUETOOTH_H

#include <QObject>
#include <QIODevice>
#include <QtBluetooth/QBluetoothSocket>

#include "abstractcomm.h"

class CommBluetooth : public AbstractComm
{
    Q_OBJECT
public:
    static const CommInfo commInfo;

    explicit CommBluetooth();

public slots:
    virtual void init() Q_DECL_OVERRIDE;
    virtual void close() Q_DECL_OVERRIDE;
    virtual void openComm(const QString &Q_NULLPTRname) Q_DECL_OVERRIDE;
    virtual void setCommProperty(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void write(const QByteArray &data) Q_DECL_OVERRIDE;

private slots:
    virtual void onRead() Q_DECL_OVERRIDE;

private:
    QBluetoothSocket *m_device;
};

#endif // COMMBLUETOOTH_H
