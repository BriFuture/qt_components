#include "commbluetooth.h"

/*!
 * \class CommBluetooth
 * \ingroup comm
 * \inherits AbstractComm
 * \author  BriFuture
 *
 * \brief 蓝牙通讯模块，由于生产环境中不支持蓝牙通讯，因此该模块暂时不可用
 */

//const CommInfo CommBluetooth::commInfo = {QObject::tr("Bluetooth"), QObject::tr("Bluetooth Mode")};

CommBluetooth::CommBluetooth()
{

}


void CommBluetooth::write(const QByteArray &data)
{
    writeData( m_device, data );
}

void CommBluetooth::init()
{
    m_device = new QBluetoothSocket( this );
    connect( m_device, &QIODevice::readyRead, this, &CommBluetooth::onRead );
}

void CommBluetooth::close()
{
    AbstractComm::close( m_device );
}


void CommBluetooth::openComm(const QString &name)
{
    Q_UNUSED(name)
//    m_device->close();
}

void CommBluetooth::setCommProperty(const QString &key, const QVariant &value)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
}


void CommBluetooth::onRead()
{
    AbstractComm::readDevice( m_device );
}
