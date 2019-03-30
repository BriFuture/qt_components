#ifndef COMMNETWORK_H
#define COMMNETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

#include <IxComm/abstractcomm.h>

class BC_DLL_EXPORT CommNetwork : public AbstractComm
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host)
    Q_PROPERTY(int port READ port)
    Q_PROPERTY(int remoteId READ remoteId)
public:
//    static const CommInfo commInfo;
    Q_INVOKABLE explicit CommNetwork();
    static const QString CommMode;

    QString host() const;
    qint16 port() const;
    int remoteId() const;

signals:

public slots:
    virtual void init() Q_DECL_OVERRIDE;
    virtual void close() Q_DECL_OVERRIDE;
    virtual void open(const QString &name) Q_DECL_OVERRIDE;
    virtual void setCommProperty(const QByteArray &key, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void write(const QByteArray &data) Q_DECL_OVERRIDE;

private slots:
    void onRead();

private:
    QString    m_host;
    qint16     m_port; // int
    QTcpSocket *m_device;

    int m_remoteId = 0;
    QByteArray m_recvedData;
};

#endif // COMMNETWORK_H
