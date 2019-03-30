#include <AbstractProtocol.h>
#include <QDebug>

/*!
 * \class AbstractProtocol
 * \ingroup comm
 * \author BriFuture
 * \brief 用于 Communicator 层的通信接口
 */

/*!
 * \fn AbstractProtocol::clearCmdCount
 * \brief 清空命令计数器
 */

/*!
 * \fn void AbstractProtocol::cmdAdded()
 *  新增加了一条指令
 */

/*!
 * \fn AbstractProtocol::skipCurrentQuery()
 * 通知通讯器跳过当前查询(查询成功)
 */

/*!
 * \fn void AbstractProtocol::stopRemainCmd()
 * 停止查询操作，清除等待的命令
 */

/*!
 * \fn void AbstractProtocol::lastQueryFailed()
 * 计时器超时后调用该方法，表明查询失败
 */

/*!
 * \fn bool AbstractProtocol::processData(const QByteArray &data, const int index=0)
 * 通讯器接收到数据时将数据转交给协议进行处理(Communicator -> Protocol)
 * @return  true 表示该协议需要独享该数据，所以数据不应该传给下一个协议；
 *          true means the protocol need to process the data exclusively,
 *          so the data should not be passed to next protocol any more;
 *          false means not;
 */

const int AbstractProtocol::DefaultMaxExecuteTimes = 1;
const int AbstractProtocol::DefaultWaitTime = 1000;  // 1000 ms
const int AbstractProtocol::CommandQueueSize = 100;  // 1000 ms

/*!
 * \fn AbstractProtocol::XOR_Code
 * \brief  求每个字符的异或值 ( #/$ 与*之间的数值的 XOR 值，不包含 #/$)
 * 根据字符计算出异或校验码
 *
 * \overload  QByteArray
 * @param  ba
 *
 * 实际例子： #BUZZER?*
 *      计算 # 和 * 之间的各个字符的校验码
 *
 * \sa XOR_CODE(const Command &)
 */
QByteArray AbstractProtocol::XOR_Code(const QByteArray &ba)
{
    char c = '\0';
    for( int i = 1; i < ba.size(); i++ ) {
        // * token means the end of valid data
        if( ba.at(i) == '*' ) {
            break;
        }
        c = ba.at(i) ^ c;
    }
    QByteArray x = QByteArray::number(c, 16);  // 十六进制表示
    // 若求出的异或值小于16（只有一位数字），在前面补0，凑足一个字节
    if( c < 0x10 ) {
        x.prepend('0');
    }
    return x;  // hex
}


AbstractProtocol::AbstractProtocol(QObject *parent) : QObject(parent)
{
    m_enabled  = true;
    m_priority = 0;
    for(int i = 0; i < CommandQueueSize; i++) {
        CommandObject *co = new CommandObject(this);
        co->setIndex(i);
        m_cmdQueue.append(co);
    }
}

/*!
 * \brief AbstractProtocol::enabled
 * \return 是否使能当前协议
 */
bool AbstractProtocol::enabled() const
{
    return m_enabled;
}

/*!
 * \brief AbstractProtocol::setEnabled
 * \param enabled 使能当前协议
 */
void AbstractProtocol::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

/*!
 * \brief AbstractProtocol::priority 优先级，数值越大，表明优先级越高
 */
int AbstractProtocol::priority() const
{
    return m_priority;
}

void AbstractProtocol::setPriority(int priority)
{
    m_priority = priority;
}

int AbstractProtocol::cmdMaxExecCount() const
{
    return m_cmdMaxExecCount;
}

/*!
 * \brief 设置指令执行次数（0 表示当前协议将会一直等待指令回复，该值一般不使用，
 * 若在网络不畅时可以使用，即等待超时时间将会被忽略）
 *
 * -1 means waitting util the command is replied
 * (so waitTime will be ignored but it still reports FAILED)
 * 0 means that command won't be repeated
 */
void AbstractProtocol::setCmdMaxExecCount(int maxExecCount)
{
    m_cmdMaxExecCount = maxExecCount;
}

int AbstractProtocol::cmdWaitTime() const
{
    return m_cmdWaitTime;
}

void AbstractProtocol::setCmdWaitTime(int millseconds)
{
    m_cmdWaitTime = millseconds;
}

/*!
 * \brief AbstractProtocol::addProcessor
 * \param header
 * \param object
 * \param method
 * 添加数据处理机制
 */
void AbstractProtocol::addProcessor(const QByteArray &header, QObject *object, const char *method)
{
    DataProcessor dp = {header, object, method};
    m_processors.append(dp);
}

void AbstractProtocol::removeProcessor(const QByteArray &header, QObject *object, const char *method)
{
    int pos = -1;
    for(int i = 0; i < m_processors.length(); i++) {
        DataProcessor &dp = m_processors[i];
        if(dp.header == header && dp.object == object && QString(method) == QString(dp.method)) {
            pos = i;
            break;
        }
    }
    if(pos > -1) {
        m_processors.removeAt(pos);
        qInfo() << "Removed processor: " << object << method ;
    }
    else {
        qWarning() << "Tried to remove invalid processor";
    }
}

/**
 * @brief AbstractProtocol::getFirstCmd
 * @return 获取队列首部的 CommandObject，移动队列
 */
CommandObject *AbstractProtocol::getFirstCmd()
{
    if(m_cmdQueueSize == 0) {
//        qWarning() << "Command Queue is full, drop command";
        return 0;
    }

    CommandObject *co = m_cmdQueue[m_cmdQueueHeader];
    m_cmdQueueHeader += 1;
    if(m_cmdQueueHeader == CommandQueueSize) {
        m_cmdQueueHeader = 0;
    }
    m_cmdQueueSize -= 1;
    return co;
}

/*!
 * \fn AbstractProtocol::addCommand
 * \param content 添加命令到等待队列中
 */
void AbstractProtocol::addCommand(const QByteArray &content)
{
    if(m_cmdQueueSize == CommandQueueSize) {
        qWarning() << "Command Queue is full, drop command";
        return;
    }
    CommandObject *co = m_cmdQueue[m_cmdQueueTail];
    co->clearExecCount();
    co->setContents(content);
    m_cmdQueueTail += 1;
    m_cmdQueueSize += 1;
    if(m_cmdQueueTail == CommandQueueSize) {
        m_cmdQueueTail = 0;
    }
    cmdAdded();
}

const int AbstractProtocol::remainCmdCount()
{
    return m_cmdQueueSize;
}

void AbstractProtocol::lastQueryFailed()
{

}

void AbstractProtocol::stopRemainCmd()
{
    m_cmdQueueHeader = m_cmdQueueTail = 0;
    m_cmdQueueSize = 0;
}

bool AbstractProtocol::processData(const DataObject &dataObj)
{
//    m_cmdQue
    dispatch(dataObj);
    return false;
}

bool AbstractProtocol::dispatch(const DataObject &data)
{
    for(int i = 0; i < m_processors.length(); i++) {
        DataProcessor &dp = m_processors[i];
        if(data.content().startsWith(dp.header)) {
            QMetaObject::invokeMethod(dp.object, dp.method, Q_ARG(DataObject, data));
            return true;
        }
    }
    return false;
}

