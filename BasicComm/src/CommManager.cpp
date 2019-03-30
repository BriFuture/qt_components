#include <CommManager.h>
#include <commimpl/CommNetwork.h>
#include <commimpl/ComFullDuplex.h>

#include <QDebug>

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


CommManager::CommManager(QObject *parent): QObject(parent)
{
    m_subThread = new QThread( this );
    m_subThread->start();  // 启动线程
}

CommManager::~CommManager() {
    if(m_currComm)
        m_currComm->setStarted(false);
//    m_currComm->close();
    m_subThread->quit();
    m_subThread->wait();
}


AbstractComm *CommManager::currentComm() const
{
    return m_currComm;
}

/*!
 * 根据给定的模式创建一个 Comm 对象
 */
AbstractComm *CommManager::createComm(const QString &mode)
{
    AbstractComm *ac = 0;
    if(mode == CommNetwork::CommMode) {
        ac = new CommNetwork();
    } else if(mode == ComFullDuplex::CommMode) {
        ac = new ComFullDuplex();
    } else {
        qWarning() << "Cannot find mode: " << mode;
        return ac;
    }

    if(ac != 0) {
        ac->moveToThread(m_subThread);
        QMetaObject::invokeMethod(ac, "init");
        return ac;
    }
    qInfo() << "Create Unknown Comm Object";

    return ac;
}

/*!
 * 查找相应模式的 Comm 对象，若找不到相应模式，则会返回 nullptr 0
 */
AbstractComm *CommManager::getComm(const QString &mode)
{
    AbstractComm *ac = 0;
    for(int i = 0; i < m_availableComms.length(); i++) {
        ac = m_availableComms[i];
        if(ac->mode() == mode) {
            return ac;
        }
    }
    return 0;
}

void CommManager::open(const QString &name, const QString &mode)
{
    if(!mode.isEmpty()) {
        AbstractComm *ac = getComm(mode);
        if(ac != 0) {
            m_currComm = ac;
        }
    }
    QMetaObject::invokeMethod(m_currComm, "open", Q_ARG(QString, name));
}

void CommManager::addComm(AbstractComm *ac)
{
    AbstractComm *curr = getComm(ac->mode());
    if(curr == 0) {
        m_availableComms.append(ac);
        ac->moveToThread(m_subThread);
        QMetaObject::invokeMethod(ac, "init");
    } else {
        qWarning() << "Add duplicated communicator whose mode is: " << ac->mode();
    }
}

void CommManager::setCommMode(const QString &mode)
{
    if(m_currComm && m_currComm->mode() == mode) {
        qInfo() << "Current communicator mode is the same as selected.";
        return;
    }
    AbstractComm *ac = getComm(mode);
    if(ac == 0) {
        ac = createComm(mode);
        if(ac == 0) {
            return;
        }
    }
    // 切换到新的模式
    m_currComm = ac;
    // 将通讯器移至子线程中，防止通讯器中可能存在的阻塞代码影响界面

    // 连接相应的信号和槽函数
    connect( m_subThread, &QThread::finished,     ac, &AbstractComm::deleteLater );
    connect( this, &CommManager::setCommProperty, ac, &AbstractComm::setCommProperty );

    // 由 recvLineData 信号触发 CommManager 的 recvLineData，涉及到多线程的问题，需要信号和槽
//    connect( ac, &AbstractComm::recvLine,      this, &CommManager::onRecvLineData );
//    connect( ac, &AbstractComm::recvRawData,   this, &CommManager::recvRawData );
//    connect( ac, &AbstractComm::sendData,      this, &CommManager::sendData );
//    connect( ac, &AbstractComm::sendData,      this, &CommManager::onWriteFinish );
//    connect( ac, &AbstractComm::stateChanged,  this, &CommManager::onStateChanged );
//    connect( ac, &AbstractComm::codeMayMessed, this, &CommManager::codeMayMessed );

    // 初始化
    // Qt 多线程机制导致不能直接调用 init 方法（不能写 m_abstractComm->init()）
    // 类似于android子线程不能直接更改UI，这里的主线程也不能直接调用子线程对象的方法，
    // 现在的 init 是在子线程中执行

    emit commChanged(ac);
}


void CommManager::startQuery()
{
    if(m_currComm->isOpened() == false) {
        // 如果下位机设备尚未打开，那么就不需要进行查询
        stopAllQuery();
//        stateChanged();
        return;
    }

    if( !m_querying ) {
        // 如果 CommManager 不在查询状态，那么从协议列表中找出有待查讯命令的协议并进行查询
        m_querying = true;
        AbstractProtocol *p;
        for( int i = 0; i < m_protocols.length(); i++ ) {
            p = m_protocols.at( i );
            if( p->remainCmdCount() > 0 ) {
                m_currProtocol = p;
                m_currCmd = p->getFirstCmd();
                execCmd();
                break;
            }
        }
    }
    calcProtocolCmd();
    emit cmdCountChanged( remainCmdCount, totalCmdCount );
}

void CommManager::stopCurrQuery()
{
    m_currCmd = 0;
}

void CommManager::stopAllQuery()
{
    for(int i = 0; i < m_protocols.length(); i++) {
        AbstractProtocol *ap = m_protocols[i];
        ap->stopRemainCmd();
    }
    m_querying = false;
}

// ***********
// Protocol
// ***********
void CommManager::addProtocol(AbstractProtocol *p)
{
    AbstractProtocol *ap;
    for(int i = 0; i < m_protocols.length(); i++) {
        ap = m_protocols[i];
        if(ap == p) {
            return;
        }
    }
    m_protocols.append(p);
}

void CommManager::delProtocol(AbstractProtocol *p)
{
    AbstractProtocol *ap;
    int pos = -1;
    for(int i = 0; i < m_protocols.length(); i++) {
        ap = m_protocols[i];
        if(ap == p) {
            pos = i;
            break;
        }
    }
    if(pos > -1) {
        qInfo() << "Protocol is removed: " << p;
        m_protocols.removeAt(pos);
    } else {
        qWarning() << "Tried to remove invalid protocol.";
    }
}

void CommManager::removeAllProtocol()
{
    m_protocols.clear();
}

const QVector<AbstractProtocol *> &CommManager::protocolList()
{
    return m_protocols;
}

void CommManager::nextQuery()
{
    if( m_currComm->isOpened() == false ) {
        stopAllQuery();
        return;
    }

    calcProtocolCmd();
//    util.timer.stop();  // 若上一条指令执行完毕，则定时器应结束计时
    // 把当前协议完成之后才能执行其他协议中的命令，
    // util.currProtocol 是一个指针，默认值是 0（空指针），其地址如 0x12345678，-> 表示解引操作符
    // util.currProtocol 由列表中的协议获得（在 CommManager::onQuery() 方法中）
    if(m_currProtocol && m_currProtocol->remainCmdCount() > 0) {
        m_currCmd = m_currProtocol->getFirstCmd();
        execCmd();
    } else if(remainCmdCount > 0) {
        // 当前协议中的命令执行完后，从协议列表中找到下一个有待执行命令的协议
        AbstractProtocol *ap;
        for( int i = 0; i < m_protocols.length(); i++ ) {
            ap = m_protocols[i];
            if( ap->remainCmdCount() > 0 ) {
                m_currProtocol = ap;
                m_currCmd = ap->getFirstCmd();
                execCmd();
                break;
            }
        }
    } else {
        // 所有的协议中的命令都执行完成
        m_querying = false; // 重置命令标志位
    }
    emit cmdCountChanged( remainCmdCount, totalCmdCount );  // 调用的槽函数 MainWindow::onCmdCountChanged，显示进度
}

void CommManager::calcProtocolCmd()
{
    int rcount = 0, tcount = 0;
    AbstractProtocol *p;
    for( int i = 0; i < m_protocols.length(); i++ ) { // util.protocolList 由两个协议，MspProtocol，NetworkProtocol
        p = m_protocols.at( i );
        rcount += p->remainCmdCount();  // 求和当前所有协议中的待执行命令数,p->remainCmdCount()取出当前指令待执行的命令数
//        tcount += p->totalCmdCount();
    }
    remainCmdCount = rcount;
//    totalCmdCount  = tcount;
}

/**
 * @brief CommManagerUtil::executeCmd
 * @return true 表示需要继续执行
 * 执行命令操作，主要是修改计数器
 */
bool CommManager::execCmd()
{
//    currProtocol->startTiming(); // 通知协议开始计时，防止命令长时间不回复
    if(!m_currCmd) {
        nextQuery();
        return false;
    }

    if( m_currCmd->execCount() <= m_currProtocol->cmdMaxExecCount() ) {
        m_currCmd->execute();
        qDebug() << "Command: " << m_currCmd->contents() << "  tried: " << m_currCmd->execCount();
        QMetaObject::invokeMethod(m_currComm, "write", Q_ARG(QByteArray, m_currCmd->contents()));
        return true;
    } else {
        m_currCmd->setState( CommandObject::Timeout );
        m_currProtocol->lastQueryFailed();
        m_currCmd = 0;
        return false;
    }
    return true;
}

/*!
 * \brief 接收到来自串口的数据后，将其转发给各个协议
 */
void CommManager::onRecvLineData(const QByteArray &data) {
//    util.lineCount++; // 统计接收到的句子的数量

    AbstractProtocol *p;
    DataObject dataObj;
    for( int i = 0; i < m_protocols.length(); i++ ) {
        p = m_protocols[i]; //
        // data 中是句子 #+600.0000*ff\r\n，按协议进行处理
        dataObj.setContent( data );
        dataObj.setIndex( 0 );
        if( p->enabled() && p->processData(dataObj) ) {
            // 这里的 processData 是回调函数(callBack)，用于处理数据
            break;
        }
    }
    emit dataRecved(dataObj);
}
