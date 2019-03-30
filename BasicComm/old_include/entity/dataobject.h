#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <QObject>
#include "bc_global.h"

class BC_DLL_EXPORT DataObject : public QObject
{
    Q_OBJECT
public:
    explicit DataObject(QObject *parent = 0);
    DataObject(const QByteArray &content, QObject *parent = 0);

    QByteArray content() const;
    void setContent(const QByteArray &content);

    int index() const;
    void setIndex(int index);

signals:

public slots:

private:
    QByteArray m_content;
    int m_index;
};

#endif // DATAOBJECT_H
