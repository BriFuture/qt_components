#ifndef COMHALFDUPLEX_H
#define COMHALFDUPLEX_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QMutex>

#include <IxComm/abstractcomm.h>

class BC_DLL_EXPORT ComHalfDuplex : public AbstractComm
{
    Q_OBJECT

public:
    static const QString CommMode;
    Q_INVOKABLE ComHalfDuplex();
    ~ComHalfDuplex();

    int blockTime() const;
    void setBlockTime(int blockTime);

    qint32 baudrate() const;

signals:

public slots:
    void init() Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;
    void open(const QString &name) Q_DECL_OVERRIDE;
    void setCommProperty(const QByteArray &key, const QVariant &value) Q_DECL_OVERRIDE;
    void write(const QByteArray &cmd) Q_DECL_OVERRIDE;

private slots:
    void onRead();

private:
    void handleQuery();
    QMutex *mutex = Q_NULLPTR;
    int m_blockTime = 3;  // 3 ms

    QByteArray m_cmd;
    QSerialPort *m_device;
    qint32 m_baudrate;
    QByteArray m_recvedData;
};

#endif // COMHALFDUPLEX_H
