#ifndef VIRTUALCOM_H
#define VIRTUALCOM_H

#include <QObject>
#include <QTimer>

#include "abstractcomm.h"

class VirtualCom : public AbstractComm
{
    Q_OBJECT
public:
//    static const CommInfo commInfo;
    static const bool existInFactory;
    Q_INVOKABLE explicit VirtualCom();
signals:

public slots:
    virtual void init() Q_DECL_OVERRIDE;
    virtual void close() Q_DECL_OVERRIDE;

    // 给定串口名，打开新的串口
    virtual void openComm(const QString &name) Q_DECL_OVERRIDE;
    virtual void setCommProperty(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void write(const QByteArray &rawdata) Q_DECL_OVERRIDE;

private slots:
    virtual void onRead() Q_DECL_OVERRIDE;
private:
    QTimer *m_timer;
    QByteArray m_data;
    int index = 0;
    int m_counter = 0;
};

#endif // VIRTUALCOM_H
