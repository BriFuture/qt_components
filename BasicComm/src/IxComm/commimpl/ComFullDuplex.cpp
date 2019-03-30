#include <commimpl/ComFullDuplex.h>
#include <qmath.h>

#include <QDebug>

/*!
 * \class ComFullDuplex
 * \ingroup comm
 * \inherits AbstractComm
 * \author  BriFuture
 * \date 2018.05.04
 *
 * \brief 管理 SerialPort，保证该程序所有类中使用的 SerialPort 对象都是统一的。
 * 串口接收到的数据和上位机程序发送到下位机的指令都通过该类传递。
 *
 * \warning 该类只负责接收和发送数据（管理上位机和下位机的通信）不会关心数据是否有错误。
 *
 * ComFullDuplex is a util to swap information with QT program
 * and SerialPort,  All data will be passed to here.
 * For convenience, the ComFullDuplex is integreted with protocol,
 * then it will conduct all commond using NMEA_0183 protocol, and
 * it is obiligate to dispatch all parsed data.
 *
 **/

const QString ComFullDuplex::CommMode = QString("serialport_fullduplex");

ComFullDuplex::ComFullDuplex() : AbstractComm()
{
    m_type = tr("SerialPort");
    m_desc = tr("Full Duplex Mode");
    m_mode = CommMode;
}

ComFullDuplex::~ComFullDuplex() {
}


void ComFullDuplex::init() {
    m_device = new QSerialPort();
    connect( m_device, &QIODevice::readyRead, this, &ComFullDuplex::onRead );
}

void ComFullDuplex::close()
{
    AbstractComm::close( m_device );
}

void ComFullDuplex::open(const QString &name) {
    m_device->close();        //关闭原来打开的串口
    // 将选中串口设置位新的串口通讯端口
    m_device->setPortName( name );
    // 尝试以读写方式打开串口
    m_opened   = m_device->open( QIODevice::ReadWrite );

    m_deviceName = name;
    m_baudrate = m_device->baudRate();
    if( m_opened ) {
        qInfo() << tr("SerialPort open port(Full Duplex): ") << name ;
    }
    else {
        m_errorString = m_device->errorString();
    }
    stateChanged();
}

void ComFullDuplex::setCommProperty(const QByteArray &key, const QVariant &value)
{
   m_device->setProperty( key, value );
}

void ComFullDuplex::write(const QByteArray &data) {
    writeData( m_device, data );
}

void ComFullDuplex::onRead()
{
//    AbstractComm::readDevice( m_device );
    QByteArray tmpData = m_device->readAll();
    emit rawDataRecved(tmpData);
    m_recvedData.append( tmpData );
    splitData(m_recvedData);
}

qint32 ComFullDuplex::baudrate() const
{
    return m_baudrate;
}
