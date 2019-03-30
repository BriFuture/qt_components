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

#include <IxComm/abstractcomm.h>

class BC_DLL_EXPORT ComFullDuplex : public AbstractComm
{
    Q_OBJECT

public: 
    static const QString CommMode;
    Q_INVOKABLE ComFullDuplex();
    ~ComFullDuplex();

    qint32 baudrate() const;

signals:

public slots:
    void init() Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;
    void open(const QString &name) Q_DECL_OVERRIDE;

    void setCommProperty(const QByteArray &key, const QVariant &value) Q_DECL_OVERRIDE;
    void write(const QByteArray &data) Q_DECL_OVERRIDE;

private slots:
    void onRead();
private:
    QSerialPort *m_device;
    qint32 m_baudrate;
    QByteArray m_recvedData;
};


#endif // COMFULLDUPLEX_H
