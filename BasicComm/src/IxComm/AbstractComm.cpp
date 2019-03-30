#include "AbstractComm.h"


/*!
 * \fn void AbstractComm::dataSend(const QByteArray &data)
 * \brief
 * 表示数据已经发送到外部设备中
 */

/*!
 * \fn void AbstractComm::init()
 * \brief
 * 初始化串口/网络，设计串口都是工作在子线程中，因此需要通过信号槽方式，
 * 在子线程中运行初始化函数
 */

/*!
 * \fn void AbstractComm::close()
 * \brief 关闭外部设备连接操作
 */

/*!
 * \fn void AbstractComm::write(const QByteArray &rawdata)
 * \brief
 * This is a funciton that actually performs the communication
 * it receives raw string and send it into serial port
 * 将文本数据写入到串口
 * 原始的文本数据
 *
 * \sa AbstractComm::writeData(QIODevice *device, const QByteArray &rawdata);
 */

/*!
 * \fn void AbstractComm::writeData
 * \brief 默认的写串口操作
 *
 * 在 rawdata 末尾添加回车换行符, 将 rawdata 发送至串口
 *
 * example:
 * \code
 *   // Assume that content of a command is "#ALARM=1*ff"
 *   // in class ImplComm
 *   writeData(m_device, content);  // then the content will be send into the device (MCU) by SerialPort/Network
 * \endcode
 */

/*!
 * \fn AbstractComm::splitData
 * \brief 按行分割数据
 *
 * example:
 * \code
 *   // in class ImplComm
 *   QByteArray data = "#abc*ff\r\n#bca*ff\r\n#cab";
 *   splitData(data);   // emit recvLine signal twice
 *   qDebug() << data; // "#cab"
 * \endcode
 */

/*!
 * \brief 默认的读取串口操作
 * 实例：查询 #SAG?*ff\r\n 接收到的数据 #+600.00000*ff\r\n
 *
 * \note 这是一个 protected 方法，不能被其它类访问，子类可以调用该方法，
 * 该方法提供了一个默认的读设备操作，若子类对设备的读取没有特殊要求，可直接调用该类
 *
 * example:
 * \code
 *   // in class ImplComm
 *   // Say, received data is "#abc*ff\r\n#bca*ff\r\n#cab", read method is invoked 5 times
 *   readDevice(m_device);  // emit recvRawData signal 5 times, recvLineData signal twice.
 * \endcode
 *
 * \sa AbstractComm::splitData
 */

const QByteArray AbstractComm::LineSeparator = QByteArray("\r\n");
const int AbstractComm::PacketMaxSize = 400;

bool AbstractComm::isOpened() const
{
    return m_opened;
}


bool AbstractComm::started() const
{
    return m_started;
}


void AbstractComm::setStarted(bool stared)
{
    m_started = stared;
}


QString AbstractComm::deviceName() const
{
    return m_deviceName;
}


QString AbstractComm::errorString() const
{
    return m_errorString;
}


QString AbstractComm::type() const
{
    return m_type;
}


QString AbstractComm::desc() const
{
    return m_desc;
}


QString AbstractComm::mode() const
{
    return m_mode;
}


void AbstractComm::close(QIODevice *device)
{
    device->close();  // 关闭设备
    m_opened = false;
    m_started = false;
    emit stateChanged();
}


void AbstractComm::writeData(QIODevice *device, const QByteArray &rawdata)
{
    if( !device->isOpen() ) {
        m_errorString = device->errorString();
        return;
    }
    // 发送数据时在数据后增加分隔符，以便下位机能够正确识别指令
    QByteArray buffer = rawdata + LineSeparator;  // #SAG?*ff\r\n
    qint64 w = device->write( buffer );
    if( w = buffer.length() ) {
        emit dataSend(buffer); // 提示管理器，数据已经发送
    }
}


void AbstractComm::splitData(QByteArray &data)
{
    if( data.length() > PacketMaxSize ) {
        // 防止乱码时找不到 \r\n 程序出现崩溃
        emit mayMess();
        data.clear();
        return;
    }
    // 假设拼接出来的句子是多个命令或者不完整。
    // 找出 \r\n 在句子中的位置，如果返回值是 -1，那么说明没有完整的句子
    int linePos = data.indexOf( LineSeparator );
    while( linePos != -1 ) {
        // not need to clear \r\n flag if it occurs on head
        // 若此时的 data 为 #600.0000*ff\r\n#500.0000*ff\r\n
        // 此时的 rawData 为 #600.0000*ff\r\n
        QByteArray rawData = data.left( linePos + 2 ); // 取出第一个回车换行左边的所有内容
        // 省略末尾的参数，取出当前位置到末尾的字符，末尾参数已省略，剩下的部分仍保存在 m_recvData 中
        // data 变成 #500.0000*ff\r\n
        data = data.mid( linePos + 2 );


        emit lineDataRecved(rawData);

        // 因为 data 还有一条语句，linePos >= 0
        linePos = data.indexOf( LineSeparator );
    }
}


