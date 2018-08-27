#include "dataobject.h"

DataObject::DataObject(QObject *parent) : QObject(parent)
{

}

DataObject::DataObject(const QByteArray &content, QObject *parent) :
    QObject( parent ),
    m_content( content )
{

}

QByteArray DataObject::content() const
{
    return m_content;
}

void DataObject::setContent(const QByteArray &content)
{
    m_content = content;
}

int DataObject::index() const
{
    return m_index;
}

void DataObject::setIndex(int index)
{
    m_index = index;
}
