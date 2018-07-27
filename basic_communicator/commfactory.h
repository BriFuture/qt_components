#ifndef COMMFACTORY_H
#define COMMFACTORY_H

#include <QObject>
#include <QVector>
#include <QMetaObject>
#include <QMap>

#include "abstractcomm.h"


class CommFactory : public QObject
{
    Q_OBJECT
    friend class CommManager;

public:
    static const QString defaultCommName;
    explicit CommFactory(QObject *parent = 0);
    CommFactory(const CommFactory &cf);
    void operator =(const CommFactory &cf);
    static CommFactory *defaultFactory();

    AbstractComm *createComm(const QString &className);

    void addComm(const QMetaObject &metaObject, const CommInfo &info);

    QList<CommInfo> commInfo();
    bool supportHalfDuplex(const QString &type);
//    const QString typeName(const QString &className);

    QVector<QMetaObject> usableComm() const;

    const QString commClassName(const QString &type, const bool halfDuplex);
signals:

public slots:

private:
    static CommFactory *m_defaultFactory;

    QVector<QMetaObject> m_usableComm;
    QMap<QString, CommInfo> m_commMap;

    QVector<AbstractComm *> m_readyComm;

};

#endif // COMMFACTORY_H
