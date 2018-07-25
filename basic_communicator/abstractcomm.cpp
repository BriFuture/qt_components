#include "abstractcomm.h"

/*!
 * \class AbstractComm
 * \inherits QObject
 * \ingroup comm
 * \author BriFuture
 * \date 2018.07.20
 *
 * \brief
 * 串口通信的接口类，所有具体实现串口通信/网络通讯的相关类都应该继承此类并实现其中的纯虚方法。
 *
 * @date 2018.04.26
 *
 * The Interface of serial communication, all implementation
 * of concrete serial communication should be the subclass
 * of this pure virtual class and implement all the virtual
 * methods in this class.
 *
 * 当串口的实现较多时，实现该类的接口时，将实现类的构造函数声明为 Q_INVOKABLE，
 * 并在相应的工厂类中进行注册，便于使用工厂方法构造子类。
 *
 * When implementing this interface, it is recommonded that
 * the constructor of subclass is decorated with the MACRO
 * Q_INVOKABLE and register it into Qt's MOC system, then
 * it will be easy to use factory to construct subclass.
 * But the port is exclusive from other threads, so be careful
 * when rewrite copy constructor if using Q_DELCARE_METATYPE to
 * declare subclass as metatype.
 *
 * @date 2018.05.09
 *
 * 因为该接口的子类受其它串口管理类的管理，并且当前程序设计是将接口的子类放入子线程中单独执行，
 * 所以实现子类时需要特别注意，子类的所有成员（QSerialPort，QTimer之类）都需要放在 init 函数中
 * 进行初始化，否则 Qt 的多线程执行时就会报错。移除具有工厂类作用的api
 *
 * Because the subclasses of this interface is managed by other COM Manager,
 * and the subclasses is designed to execute into a sub thread, when realizing
 * subclass of this interface should remember that all member of certain subclass
 * should be initialized in Function init, otherwise the Qt's multi-thread mechansim
 * would raise error.
 * Apis that effect like factory are removed now。
 *
 * \sa CommManager
 */

const QByteArray AbstractComm::LineSeparator = QByteArray("\r\n");
const int AbstractComm::PacketMaxSize = 400;
const CommInfo AbstractComm::commInfo = {QObject::tr( "AbstractComm" )};
CommState AbstractComm::m_state;

AbstractComm::AbstractComm() : QObject()  {
    m_recvData.reserve(PacketMaxSize);
}


/*!
 * \brief  返回当前串口/网络设备是否正在运行的标志位
 *
 * example:
 * \code
 *   AbstractComm *ac = new ImplComm;
 *   ac->openDevice("device");  // 打开设备
 *   ac->isStarted();  // true
 *   ac->close();
 *   ac->isStarted();  // false
 * \endcode
 */
bool AbstractComm::isStarted() {
    return m_started;
}

/*!
 * \brief 设置标志位，表示串口/网络设备是否可以运行（由控制器设置，但实现类可忽略该标志位）
 *
 * example:
 * \code
 *   AbstractComm *ac = new ImplComm;
 *   ac->setStarted(true);  // 设置标志位
 *   ac->isStarted();  // true
 * \endcode
 *
 * \note you need to have the flag set when opening a device by default.
 * or the manager may not set the flag to false.
 *
 * \code
 *   AbstractComm *ac = new ImplComm;
 *   ac->openDevice("COM1");
 *   ac->isStarted();  // true
 * \endcode
 *
 * \sa AbstractComm::openDevice
 */
void AbstractComm::setStarted(const bool started) {
    m_started = started;
}


/*!
 * \brief 默认的关闭串口/网络操作
 * 提供统一的关闭操作，供子类调用，若子类对关闭操作没有特殊的要求，那么不需要重写关闭操作
 */
void AbstractComm::close(QIODevice *device)
{
    device->close();  // 关闭设备
    m_state.opened   = false;
    emit stateChanged( m_state );
}

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
void AbstractComm::readDevice(QIODevice *device)
{
    // @Note must use readAll() to get all data in case of multiple \r\n remains in BUFFER
    m_tmpData = device->readAll();
    emit recvRawData( m_tmpData );
    m_recvData.append( m_tmpData );
    splitData();
}

/*!
 * \brief AbstractComm::splitData 按行分割数据
 * \note 该接口的实现类若想使用该方法进行默认的分割数据操作，那么子类中不能重新定义 m_recvData
 *
 * example:
 * \code
 *   // in class ImplComm
 *   m_recvData = "#abc*ff\r\n#bca*ff\r\n#cab";
 *   splitData();   // emit recvLine signal twice
 *   qDebug() << m_recvData; // "#cab"
 * \endcode
 */
void AbstractComm::splitData()
{
    // 假设拼接出来的句子是多个命令或者不完整。
    // 找出 \r\n 在句子中的位置，如果返回值是 -1，那么说明没有完整的句子
    int linePos = m_recvData.indexOf( LineSeparator );
    while( linePos != -1 ) {
        // not need to clear \r\n flag if it occurs on head
        // 若此时的 m_recvData 为 #600.0000*ff\r\n#500.0000*ff\r\n
        // 此时的 rawData 为 #600.0000*ff\r\n
        QByteArray rawData = m_recvData.left( linePos + 2 ); // 取出第一个回车换行左边的所有内容
        // 省略末尾的参数，取出当前位置到末尾的字符，末尾参数已省略，剩下的部分仍保存在 m_recvData 中
        // m_recvData 变成 #500.0000*ff\r\n
        m_recvData = m_recvData.mid( linePos + 2 );

        if( m_recvData.length() > PacketMaxSize ) {
            // 防止乱码时找不到 \r\n 程序出现崩溃
            emit codeMayMessed();
            m_recvData.clear();
        }
        emit recvLine( rawData );

        // 因为 m_recvData 还有一条语句，linePos >= 0
        linePos = m_recvData.indexOf( LineSeparator );
    }
}

/*!
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
void AbstractComm::writeData(QIODevice *device, const QByteArray &rawdata)
{
    if( !device->isOpen() ) {
        m_state.errorString = device->errorString();
        return;
    }
    // 发送数据时在数据后增加分隔符，以便下位机能够正确识别指令
    QByteArray buffer = rawdata + LineSeparator;  // #SAG?*ff\r\n
    qint64 w = device->write( buffer );
    if( w = buffer.length() ) {
        emit sendData( buffer ); // 提示管理器，数据已经发送
    }
}

/*!
 * \brief 设置状态，注意，所有的子类都共享抽象类里的 static 内存区域，
 * 对某个子类的 state 进行修改，会影响到其它所有的子类。
 *
 * example:
 * \code
 *   AbstractComm *cfd = new ImplComm1;
 *
 *   CommState s1 = cfd->state();
 *   qDebug() << s1; // { "COM1", 9600, false, "" }
 *
 *   s1.baudRate = 38400;
 *   cfd->setState(s1);
 *
 *   AbstractComm *chd = new ImplComm1;
 *   CommState s2 = cfd->state();
 *   qDebug() << s2; // { "COM1", 38400, false, "" }
 * \endcode
 *
 * \sa AbstractComm::state()
 */
void AbstractComm::setState(const CommState &state)
{
    m_state = state;
}

CommState &AbstractComm::state()
{
    return m_state;
}

/*!
 * \fn void AbstractComm::sendData(const QByteArray &data)
 * \brief
 * 向界面发送数据
 */

/*!
 * \fn void AbstractComm::init()
 * \brief
 * 初始化串口/网络，设计串口都是工作在子线程中，因此需要通过信号槽方式，
 * 在子线程中运行初始化函数
 */

/*!
 * \fn void AbstractComm::openComm(const QString &name)
 * \brief
 * 根据串口/网络名称，打开新的串口/网络
 *
 * example:
 * \code
 *   AbstractComm *cfd = new ImplComm1;
 *   cfd->openComm("COM1");  // 打开串口
 * \endcode
 */

/*!
 * \fn void AbstractComm::setCommProperty(const QString &key, const QVariant &value)
 * \brief
 * 键和值需要参考 QSerialPort 的 Property
 * 设置串口属性（波特率，停止位等）、网络属性（地址）等
 *
 * example:
 *   AbstractComm *cfd = new ImplComm1;
 *   cfd->setCommProperty("baudRate", 2400);  // 设置波特率为 2400
 *   cfd->setCommProperty("dataBit",  0);  // 设置数据位为 8
 * \endcode
 */

/*!
 * \fn void AbstractComm::write(const QByteArray &rawdata)
 * \brief
 * This is a funciton that actually performs the communication
 * it receives raw string and send it into serial port
 * 将文本数据写入到串口
 * 原始的文本数据
 */

/*!
 * \fn void AbstractComm::onRead()
 * \brief
 * 读取操作，若运行在子线程的循环中，需要显式调用 Qt 的事件分发，
 * 避免子线程无法关闭。
 */

/*!
 * \fn void AbstractComm::recvLine(const QByteArray &data)
 * \brief 从串口/网络等设备中接收到的一行数据。
 * 行结束符为 LineSeparator
 *
 */

/*!
 * \fn void AbstractComm::recvRawData(const QByteArray &data)
 * \brief 从串口/网络等设备中接收到的数据，该数据是一行数据中的一部分。
 *
 */

/*!
 * \struct CommState 表示串口/网口状态信息等
 * 包括设备名，是否打开，波特率等信息
 *
 */

/*!
 * \struct CommInfo 表示通讯器的信息
 * 包括通讯器类型，描述文本，是否支持半双工通讯等
 *
 */

QDebug operator<<(QDebug debug, const CommInfo &ci)
{
    debug << "{ " <<ci.type << ", " << ci.desc << ", " << ci.isHalfDuplex << " }";
    return debug;
}

QDebug operator<<(QDebug debug, const CommState &cs)
{
    debug << "{ " <<cs.deviceName << ", " << cs.baudRate << ", " << cs.opened << ", " << cs.errorString << " }";
    return debug;
}
