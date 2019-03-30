#include "commnetwork.h"

/*!
 * \class  CommNetwork
 * \ingroup comm
 * \inherits AbstractComm
 * \author BriFuture
 *
 * \brief 网络连接方式，接收服务器数据或将数据发送到相应的服务器端口中
 *
 * 与其他连接方式不同，该方式使用 TcpSocket 连接到远程服务器中（而不是常用的串口），
 * 但由于 TcpSocket 继承自 QIODevice，因此实现方式与串口的实现类大同小异
 */
//bject::tr("Network Mode")};
const QString CommNetwork::CommMode = QString("network_fullduplex");
CommNetwork::CommNetwork() : AbstractComm()
{
    //    connectToHost( QHostAddress("203.195.160.208"), DefaultServerPort );
    m_type = tr("network");
    m_desc = tr("Network Mode(socket is used to communicate)");
    m_mode = CommMode;
}

void CommNetwork::init() {
    m_device = new QTcpSocket( this );
    connect( m_device, &QIODevice::readyRead, this, &CommNetwork::onRead );
}

void CommNetwork::close()
{
    AbstractComm::close( m_device );
}

/*!
 * \brief 根据主机名（ip地址）连接主机
 * name 一般为 IP 或主机名
 */
void CommNetwork::open(const QString &name) {
    m_device->close();  // 首先关掉之前连接的服务器
    m_device->connectToHost( QHostAddress( name ), m_port );  // m_port 是服务器上的端口

    m_opened     = m_device->isOpen();
    m_deviceName = name;
    if( m_opened ) {
        qInfo()<< tr("connect to host: ") << name;
//        query( QString("PC-CN=%1\r\n").arg( m_remoteId ).toLatin1() );
    }
    else {
        m_errorString = m_device->errorString();
    }
    stateChanged(); // 通知界面状态改变
}

void CommNetwork::write(const QByteArray &data) {
    // Q_ARG 是 Arguments
//    QMetaObject::invokeMethod( this, "query", Q_ARG( QByteArray, cmd ) ); // 确保在子线程中执行 query 方法
    writeData( m_device, data);  // 将 data 发送的设备 m_device
}

void CommNetwork::setCommProperty(const QByteArray &key, const QVariant &value) {
    if( key == "port" ) {
        m_port = value.toInt();  // 在界面中的 6666 赋值给 m_port
    }
    else if( key == "remoteId" ) {
        m_remoteId = value.toInt(); // 对应的下位机的 id
    }
}


void CommNetwork::onRead() {
    QByteArray tmpData = m_device->readAll();
    emit rawDataRecved(tmpData);
    m_recvedData.append( tmpData );
    splitData(m_recvedData);
}

int CommNetwork::remoteId() const
{
    return m_remoteId;
}

qint16 CommNetwork::port() const
{
    return m_port;
}

QString CommNetwork::host() const
{
    return m_host;
}
