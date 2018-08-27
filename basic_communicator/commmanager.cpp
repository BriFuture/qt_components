#include "commmanager.h"
#include "commfactory.h"
#include <QDebug>

/*!
 * \class CommManager
 * \inherits QObject
 * \ingroup comm
 * \author BriFuture
 * \brief 通讯管理器，传递查询语句和结果管理串口子线程的作用，封装了多线程，向外部提供统一的
 * 接口，起到类似中介的作用，将 AbstractProtocol 和实际执行串口/网络访问的类（子线程中）聚合起来，方便迭代更新
 *
 * \date 2018.07.19
 * This is a new Communicator Class which doesn't  exactly query and process data,
 * because the real serialPort/socket is moved into a subThread which wont block UI-thread,
 * it is a manager class that handle the sub thread, notify serial to query, receive data
 * from serial port / network, and transfer it into BaseComm's implementation.
 *
 * \warning \date 2018.04.25
 *
 * 此前串口通讯无法正确识别下位机的发送时间间隔（数据每隔 16 ms 才被接收到）。最终在 FT232R 的驱动设置中找到了延时的根本原因
 * （参考\l {http://projectgus.com/2011/10/notes-on-ftdi-latency-with-arduino/}），
 * 因此解决了串口通讯无法识别下位机发送时间间隙的问题。
 *
 * Because of the delay of data coming into serial port (it is after almost 16ms that
 * the data can be received from serial port), Finally, we found the root cause of
 * the delay is made by the driver settings of FT232R with the help of this article:
 * \l {http://projectgus.com/2011/10/notes-on-ftdi-latency-with-arduino/}
 * At last, we solved the problem of the data delay and successfully found the gap
 * between tow sentences from slave computer.
 *
 * example:
 * \code
 *   CommManager *cm = Comm::manager;
 *   cm.resetMode("Network", true);  // 设为网络通讯半双工模式
 *   connect(cm, &CommManager::recvLineData, this, &SomeClass::recvLine);  // 接收数据
 * \endcode
 */

CommManager::CommManager(QObject *parent) : QObject(parent)
{
    // 在 main thread (GUI thread) 新建子线程，在子线程中执行通讯相关的代码
    m_subThread = new QThread( this );
    m_subThread->start();  // 启动线程
    util.timer.setSingleShot( true );
    connect(&util.timer, &QTimer::timeout, this, &CommManager::onTimeout);
    // 初始化串口，根据传入的字符串打开指定模式的串口
    init( CommFactory::defaultCommName );
}


/*!
 * \brief  将 mode 字符串传递给负责构造 AbstractComm 的工厂类。
 * Pass the mode string into CommFactory::defaultFactory() that responds for construct BaseComm
 * @param mode
 */
void CommManager::init(const QString &name) {
    // m_ac 负责控制实际的串口通讯
    m_abstractComm = CommFactory::defaultFactory()->createComm( name );
    // 保存通讯器名称
    util.commClassName = name;

    // 将通讯器移至子线程中，防止通讯器中可能存在的阻塞代码影响界面
    m_abstractComm->moveToThread( m_subThread );

    // 连接相应的信号和槽函数
    connect( m_subThread, &QThread::finished,     m_abstractComm, &AbstractComm::deleteLater );
    connect( this, &CommManager::setCommProperty, m_abstractComm, &AbstractComm::setCommProperty );

    // 由 recvLineData 信号触发 CommManager 的 recvLineData，涉及到多线程的问题，需要信号和槽
    connect( m_abstractComm, &AbstractComm::recvLine,      this, &CommManager::onRecvLineData );
    connect( m_abstractComm, &AbstractComm::recvRawData,   this, &CommManager::recvRawData );
    connect( m_abstractComm, &AbstractComm::sendData,      this, &CommManager::sendData );
    connect( m_abstractComm, &AbstractComm::sendData,      this, &CommManager::onWriteFinish );
    connect( m_abstractComm, &AbstractComm::stateChanged,  this, &CommManager::onStateChanged );
    connect( m_abstractComm, &AbstractComm::codeMayMessed, this, &CommManager::codeMayMessed );

    // 初始化
    // Qt 多线程机制导致不能直接调用 init 方法（不能写 m_abstractComm->init()）
    // 类似于android子线程不能直接更改UI，这里的主线程也不能直接调用子线程对象的方法，
    // 现在的 init 是在子线程中执行
    QMetaObject::invokeMethod( m_abstractComm, "init" );  // 调用 m_abstractComm 的 init 方法

    emit commInfoChanged(currentCommInfo());
}

/*!
 * \brief 返回当前所用的通讯器
 */
AbstractComm *CommManager::abstractComm() const
{
    return m_abstractComm;
}

/*!
 * \brief 指定的className，重设Comm模式
 * 提供该接口是为了在某些特殊情况下，现有的通讯器无法满足要求，需要扩展通讯器时，通过该方法生成自定义的通讯器类
 *
 * \overload
 *
 * 扩展通讯器示例：
 * \code
 *   Comm::factory->addComm(SubComm::staticMetaObject, SubComm::commInfo);  // SubComm is a subclass of Abstract Comm
 *   Comm::manager->resetMode("SubComm");  // 构建一个新的通讯器
 * \endcode
 *
 * common example:
 * \code
 *   // assume that there is a communicator VirtualCom OTHER THAN ComFullDuplex
 *   CommManager *manager = Comm::manager;
 *   ...
 *   manager->resetMode("VirtualCom");  // Data will be received from VirtualCom
 * \endcode
 *
 * \sa CommFactory CommManager::resetMode(const QString &type, const bool halfDuplex)
 */
void CommManager::resetMode(const QString &className) {
    //  如果重设的通讯器和当前正在使用的通讯器相同，那么不需要重新构造通讯器
    if( className == util.commClassName ) {
        return;
    }
    close();
    init( className );    // 根据通讯器名称重新进行初始化
    qInfo() << "CommManager mode changed to " << util.commClassName;
    if( !m_state.deviceName.isEmpty() ) {
        openDevice( m_state.deviceName );  // 打开设备
    }
}

/*!
 * \brief CommManager::resetMode
 *
 * \overload
 *
 * example:
 * \code
 *   ...
 *   manager->resetMode("Network", false);  //设为网络通讯全双工模式s
 * \endcode
 */
void CommManager::resetMode(const QString &type, const bool halfDuplex) {
    QString name = CommFactory::defaultFactory()->commClassName(type, halfDuplex);
    resetMode(name);
}

/*!
 * \brief 关闭串口/网路，结束子线程
 */
CommManager::~CommManager() {
    m_abstractComm->setStarted( false );
    m_subThread->quit();
    m_subThread->wait();
}


/*!
 * \brief 关闭串口并清空指令队列
 *
 * example:
 * \code
 *   CommManager *m = Comm::manager;
 *   m->openDevice("COM1");  // 打开 COM1 串口
 *   ...
 *   m->close();  // 关闭打开的 COM1 串口
 * \endcode
 */
void CommManager::close() {
    m_abstractComm->setStarted( false );
    stopAllQuery();    // 停止所有查询
    QMetaObject::invokeMethod( m_abstractComm, "close" ); // 并关闭子线程中的通讯器，在子线程中执行 abstractComm 的 close 方法
}

/*!
 * 向 CommManager 管理器中添加协议，以便处理相应协议的数据
 *
 * example:
 * \code
 *   CommManager *manager = Comm::manager;
 *   ...
 *   qDebug() << "List Size: " << manager.protocolList().size();  // assume the size now is N = 10, then output is "List Size: 10"
 *   AbstractProtocol *p  = new ImplProtocol;  // ImplProtocol must be implement subclass of AbstractProtocol
 *   manager->addProtocol(p);  // now ImplProtocol is added, the data will be transmitted to this protocol if it is enabled.
 *   qDebug() << "List Size: " << manager.protocolList().size();  // output is "List Size: 11"
 * \endcode
 *
 * \sa CommManager::onRecvLineData  CommManager::delProtocol
 */
void CommManager::addProtocol(AbstractProtocol *p) {
    // 连接 AbstractProtocol 的 skipCurrentQuery 信号，通知 CommManager 停止执行当前指令并执行下一条命令
    connect( p, &AbstractProtocol::cmdAdded, this, &CommManager::startQuery );
    connect( p, &AbstractProtocol::skipCurrentQuery, this, &CommManager::startNextQuery );
    util.protocolList.append( p );
}

/*!
 * \brief 删除管理器中的协议
 *
 * example:
 * \code
 *   CommManager *manager = Comm::manager;
 *   ...
 *   qDebug() << "List Size: " << manager.protocolList().size();  // assume the size now is N = 10, then output is "List Size: 10"
 *   manager->delProtocol(p);  // p points to an instance of ImplProtocol
 *   qDebug() << "List Size: " << manager.protocolList().size();  // output is "List Size: 9"
 * \endcode
 *
 * \sa CommManager::addProtocol
 */
void CommManager::delProtocol(AbstractProtocol *p)
{
    util.protocolList.removeOne(p);
}

void CommManager::removeAllProtocol()
{
    util.protocolList.clear();
}

/*!
 * \fn CommManager::protocolList() const
 * @return 当前 CommManager 管理器中保存有的协议
 */
const QList<AbstractProtocol *> &CommManager::protocolList()
{
    return util.protocolList;
}

/*!
 * \brief 打开指定设备，若参数 name 为空，则默认打开最后一次使用的设备
 *
 * example:
 * 打开串口 COM1
 * \code
 *   CommManager *manager = Comm::manager;
 *   manager->openDevice("COM1");  // if serial port COM1 is free to use, then COM1 will be occupied by the program
 * \endcode
 *
 * 连接服务器 192.168.0.100 / localhost，连接网络前需要设置端口
 * \code
 *   manager->setCommProperty("port", 8080);
 *   manager->openDevice("localhost");  // connect to localhost
 * // manager->openDevice("192.168.0.100");  // effects the same as before
 * \endcode
 *
 * \sa CommManager::setCommProperty CommManager::close
 */
void CommManager::openDevice(const QString &name) {
    if( !name.isEmpty() ) {
        m_state.deviceName = name;
    }
    m_abstractComm->setState(m_state);
    QMetaObject::invokeMethod( m_abstractComm, "openComm",
                               Q_ARG( QString, m_state.deviceName )
                              );  // 异步调用，在子线程中执行 abstractComm 的 openComm 方法
}


/*!
 * \brief 发送的指令可能不会立即被下位机执行，需要等待一定时间接收下位机的反馈。
 *
 * example
 * \code
 *   ...
 *   manager->startQuery();  // 从协议列表中找出一个用命令的协议，将该命令写入到串口等设备中进行查询
 * \endcode
 */
void CommManager::startQuery() {
    // 如果下位机设备尚未打开，那么就不需要进行查询
    if( m_state.opened == false ) {
        stopAllQuery();
        m_state.errorString = tr("Device is not open.");
        stateChanged(m_state);
        return;
    }

    if( !util.querying ) {
        // 如果 CommManager 不在查询状态，那么从协议列表中找出有待查讯命令的协议并进行查询
        util.querying = true;
        AbstractProtocol *p;
        for( int i = 0; i < util.protocolList.length(); i++ ) {
            p = util.protocolList.at( i );
            if( p->remainCmdCount() > 0 ) {
                util.currProtocol = p;
                util.currCmd = p->getFirstCmd();
                write();
                break;
            }
        }
    }
    util.calcProtocolCmd();
    emit cmdCountChanged( util.remainCmdCount, util.totalCmdCount );
}

/*!
 * \brief 若协议中有指令未执行，接着执行下一条指令
 */
void CommManager::startNextQuery() {
    if( m_state.opened == false ) {
        stopAllQuery();
        return;
    }

    util.calcProtocolCmd();
    util.timer.stop();  // 若上一条指令执行完毕，则定时器应结束计时
    // 把当前协议完成之后才能执行其他协议中的命令，
    // util.currProtocol 是一个指针，默认值是 0（空指针），其地址如 0x12345678，-> 表示解引操作符
    // util.currProtocol 由列表中的协议获得（在 CommManager::onQuery() 方法中）
    AbstractProtocol *p = util.currProtocol;
    if( p && (p->remainCmdCount() > 0) ) {        // wait current AbstractProtocol finish
        util.currCmd = p->getFirstCmd();
        write();
    }
    else if( util.remainCmdCount > 0 ) {
        // 当前协议中的命令执行完后，从协议列表中找到下一个有待执行命令的协议
        for( int i = 0; i < util.protocolList.length(); i++ ) {
            p = util.protocolList.at( i );
            if( p->remainCmdCount() > 0 ) {
                util.currProtocol = p;
                util.currCmd = p->getFirstCmd();
                write();
                break;
            }
        }
    }
    else {
        // 所有的协议中的命令都执行完成
        util.querying = false;   // 重置命令标志位
        AbstractProtocol *p;
        for( int i = 0; i < util.protocolList.length(); i++ ) {
            p = util.protocolList.at( i );
            p->clearCmdCount();  // 释放内存，清空指队列和清零计数器
        }
    }
    emit cmdCountChanged( util.remainCmdCount, util.totalCmdCount );  // 调用的槽函数 MainWindow::onCmdCountChanged，显示进度
}


/*!
 * \fn CommManager::stopAllQuery()
 * 停止所有查询，并通知协议管理器也重置查询
 */
void CommManager::stopAllQuery() {
    util.timer.stop();  // 停止计时
    // 通知协议管理器停止查询
    for( int i = 0; i < util.protocolList.length(); i++ ) {
        util.protocolList.at( i )->stopRemainCmd();
    }
    util.querying = false;
    // 停止正在执行的查询，防止下位机响应过慢导致正在查询的命令无法清除
//    startNextQuery();
}

void CommManager::write()
{
    QMetaObject::invokeMethod( m_abstractComm, "write", Q_ARG(QByteArray, util.currCmd->contents() ) );
}


/*!
 * \brief 在实际串口执行完查询后进行相应操作
 *
 * @date 2018.06.16 可能存在的bug：
 * 假设查询队列为 Q1, Q2, Q3
 * 回复为 A1, A2 A3, A4 这里的 A2 A3 是在查询完 Q2 之后同时回复
 * 然而此时协议的定时器尚未工作，在接收到 A3 回复时由于定时器发生错误导致查询队列（程序）崩溃，
 * 因此协议在接收指令时务必判断定时器是否工作
 */
void CommManager::onWriteFinish() {
    util.timer.start( util.currCmd->getTimeout() );
    util.cmdExecuteTimes ++;
    util.currCmd->setState( CommandObject::Executing );
}

void CommManager::onTimeout()
{
    if( util.executeCmd() ) {  // 命令需要继续执行
        write(); // 命令已执行过，将其写入到设备，等待重新计时
    }
    else {  // 上一条命令已经执行完毕
        startNextQuery();
    }
}


/*!
 * \brief  在设备的状态改变时，将状态转发给其它层
 */
void CommManager::onStateChanged(const CommState &state) {
    m_state = state;
    emit stateChanged( m_state );
}

/*!
 * \fn CommManager::getProtocol()
 * \overload
 *
 * 一个可接受类型的重载函数可直接返回需要的具体协议类型的指针
 * \sa CommManager::AbstractProtocol(const QString &name)
 */

/*!
 * \fn CommManager::AbstractProtocol(const QString &name)
 * \overload
 * 通过对象 name 找到相应的 AbstractProtocol 指针
 *
 * example:
 * \code
 *   ImplProtocol *ip = static_cast<ImplProtocol *>(manager->protocol("impl_protocol"));
 * // it's equivalent to the following
 *   ImplProtocol *ip = manager->getProtocol<ImplProtocol *>("impl_protocol");
 * \endcode
 *
 * \sa CommManager::getProtocol()
 */
AbstractProtocol *CommManager::protocol(const QString &objectName) {
    AbstractProtocol *p = Q_NULLPTR;
    // 如果没有找到相应的协议，就返回一个无效的指针
    AbstractProtocol *pFound = Q_NULLPTR;
    for( int i = 0; i < util.protocolList.length(); i++ ) {
        p = util.protocolList.at( i );
        if( p->objectName() == objectName ) {
            pFound = p;
            break;
        }
    }
    return pFound;
}

/*!
 * \brief 返回当前通讯器的 CommInfo
 * \return
 */
const CommInfo &CommManager::currentCommInfo()
{
    QMapIterator<QString, CommInfo> it(CommFactory::defaultFactory()->m_commMap);
    while(it.hasNext()) {
        it.next();
        if(it.key() == util.commClassName) {
            return it.value();
        }
    }
    return AbstractComm::commInfo;
}


/*!
 * \brief 返回串口状态
 */
const CommState &CommManager::state()
{
   return m_state;
}


/*!
 * \brief 接收到来自串口的数据后，将其转发给各个协议
 */
void CommManager::onRecvLineData(const QByteArray &data) {
    util.lineCount++; // 统计接收到的句子的数量
    emit recvLineData(data, util.lineCount);

    AbstractProtocol *p;
    DataObject dataObj;
    for( int i = 0; i < util.protocolList.length(); i++ ) {
        p = util.protocolList.at( i ); // at(0) 为 MspProtocol 的对象
        // data 中是句子 #+600.0000*ff\r\n，按协议进行处理
        dataObj.setContent( data );
        dataObj.setIndex( util.lineCount );
        if( p->enabled() && p->processData( &dataObj ) ) {  // 这里的 processData 是回调函数，用于处理数据 (callBack)
            break;
        }
    }
}

// ================================
/*!
 * \fn CommManager::cmdCountChanged(const int remain, const int all)
 * 通知其他组件指令数目有变化
 */

/*!
 * \fn CommManager::recvRawData(const QByteArray &data)
 * 通知其他组件从串口/网络中接收到的原始数据（即不一定包含 AbstractProtocol::LineSeperator 的数据）
 */

/*!
 * \fn CommManager::recvLineData(const QByteArray &data);
 * 通知其他组件从串口/网络中接收到的一行数据（即一定包含 AbstractProtocol::LineSeperator 的数据）
 */

/*!
 * \fn CommManager::sendData(const QByteArray &data)
 * 表示数据已经由通讯器发送到串口/网络中
 */

/*!
 * \fn CommManager::stateChanged(const CommState &state)
 * 通知串口/网络设备的状态有变化
 */

/*!
 * \fn CommManager::setCommProperty(const QString &key, const QVariant &value);
 * 设置串口/网络设备的属性
 * \sa AbstractComm::setCommProperty
 */

// =======================================

/*!
 * \class CommManagerUtil
 * \author BriFuture
 * \date 2018.07.19
 * \brief 助手类，统计接收到的语句、指令队列中的待查询指令数
 *
 */

/*!
 * \brief
 * @return 当前所有协议（包括 MSP 协议和网络服务器协议或指令集）的指令队列中等待查询的命令数
 */
void CommManagerUtil::calcProtocolCmd()
{
    int rcount = 0, tcount = 0;
    AbstractProtocol *p;
    for( int i = 0; i < protocolList.length(); i++ ) { // util.protocolList 由两个协议，MspProtocol，NetworkProtocol
        p = protocolList.at( i );
        rcount += p->remainCmdCount();  // 求和当前所有协议中的待执行命令数,p->remainCmdCount()取出当前指令待执行的命令数
        tcount += p->totalCmdCount();
    }
    remainCmdCount = rcount;
    totalCmdCount  = tcount;
}

/**
 * @brief CommManagerUtil::executeCmd
 * @return true 表示需要继续执行
 * 执行命令操作，主要是修改计数器
 */
bool CommManagerUtil::executeCmd()
{
//    currProtocol->startTiming(); // 通知协议开始计时，防止命令长时间不回复

    if( !currCmd || currCmd->state() >= CommandObject::Timeout )
        return false;

    qDebug() << "Command: " << currCmd->contents() << "  tried: " << cmdExecuteTimes;
    if( currCmd->getMaxExecuteTimes() <= cmdExecuteTimes ) {
        cmdExecuteTimes = 0;
        currCmd->setState( CommandObject::Timeout );
        currProtocol->lastQueryFailed();
        currCmd = 0;
        return false;
    }
    return true;
}
