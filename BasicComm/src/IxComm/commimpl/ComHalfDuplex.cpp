#include <ComHalfDuplex.h>

#include <QCoreApplication>
#include <QDebug>


/*!
 * \class ComHalfDuplex
 * \ingroup comm
 * \inherits AbstractComm
 * \author  BriFuture
 * \date    2018.04.27
 *
 * \brief 半双工工作模式的串口
 *
 * 解决上位机发送指令和下位机发送数据同时发生时产生冲突导致
 * 乱码的问题，解决冲突的主要原理是使用 waitForReadyRead 方法
 * 对串口进行阻塞式读取，超时后即认为当前串口未被下位机占用，
 * 此时上位机可以发送指令。
 * （显然，用 readyRead 信号的异步方式也是可以完成相同的任务的，
 * 只是当想清楚这点时，已经写好了这个类，并且重新构建了 UI-Protocol
 * -Com 之间的结构，因此并未改动 ComFullDuplex 中的代码）
 *
 * This class is made to solve the problem that occurs
 * when master computer(PC) send instruction into serial port
 * while slave computer send data into serial port and they
 * conflict with each other. The problem is now solved by
 * using the blocking api waitForReadyRead to read the
 * serial port. If blocking api triggers timeout event,
 * it will send the command waiting to be send into serial port.
 * (Obviously it can be replaced with non-blocking api
 * which is driven by readyRead signal, but before it occurs
 * to me that the non-blocking idea can also be used to solve
 * the data conflict problem, I have finished this class and
 * re-implement the structure of UI-Protocol-Com, So I didn't
 * change the code in ComFullDuplex)
 */

//const CommInfo ComHalfDuplex::commInfo = {QObject::tr("SerialPort"), QObject::tr("Half Duplex Mode"), true};
const QString ComHalfDuplex::CommMode = QString("serialport_halfduplex");
ComHalfDuplex::ComHalfDuplex() : AbstractComm()
{
    m_type = tr("SerialPort");
    m_desc = tr("Half Duplex Mode");
    m_mode = CommMode;
}

ComHalfDuplex::~ComHalfDuplex() {
    m_started = false;
    delete mutex;
}


void ComHalfDuplex::init() {
    m_device = new QSerialPort( this );
    mutex  = new QMutex();
}


void ComHalfDuplex::close() {
    m_started = false;
    QMetaObject::invokeMethod(this, "close", Q_ARG(QIODevice *,m_device));
}


void ComHalfDuplex::open(const QString &name) {
    m_device->close();        //关闭原来打开的串口
    // 将选中串口设置位新的串口通讯端口
    m_device->setPortName( name );
    m_started = m_device->open( QIODevice::ReadWrite );

    m_deviceName = name;
    m_opened   = m_started;
    m_baudrate = m_device->baudRate();
    if( m_started ) {
        qInfo() << tr("SerialPort open port(Half Duplex): ") << name ;
        stateChanged();
        while( m_started ) {
            onRead();
        }
    }
    else {
        m_errorString = m_device->errorString();
        stateChanged();
    }
}

void ComHalfDuplex::setCommProperty(const QByteArray &key, const QVariant &value)
{
    m_device->setProperty( key, value );
}

/*!
 * \brief
 * this is a blocked function which will block until new data coming
 * It will cost at least 5ms when re-calling this function, so if a
 * counter reached about 2 (from 0) indicates that there is no more
 * data 10ms before.
 */
void ComHalfDuplex::onRead() {
    QByteArray tmpData;
    while( m_device->waitForReadyRead( m_blockTime ) ) {
        tmpData = m_device->readAll();
        m_recvedData.append( tmpData );
        emit rawDataRecved(tmpData);
    }

    handleQuery();
    splitData(m_recvedData);
    QCoreApplication::processEvents();
}

/**
 * @brief ComHalfDuplex::writeCmd
 * @param cmd
 * 保存指令，指令不是立即发送到串口，等到串口空闲时再发送指令
 */
void ComHalfDuplex::write(const QByteArray &cmd) {
    mutex->lock();
    m_cmd = cmd;
    mutex->unlock();
}

void ComHalfDuplex::handleQuery() {
    if( !m_cmd.isEmpty() ) {
        writeData( m_device, m_cmd );
        mutex->lock();
        m_cmd.clear();
        mutex->unlock();
    }
}

qint32 ComHalfDuplex::baudrate() const
{
    return m_baudrate;
}

/*!
 * \brief 返回阻塞的等待时间
 */
int ComHalfDuplex::blockTime() const
{
    return m_blockTime;
}

/*!
 * \brief 设置阻塞时的等待时间
 */
void ComHalfDuplex::setBlockTime(int blockTime)
{
    m_blockTime = blockTime;
}
