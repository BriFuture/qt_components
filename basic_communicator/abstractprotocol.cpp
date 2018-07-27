#include "abstractprotocol.h"

/*!
 * \class AbstractProtocol
 * \ingroup comm
 * \author BriFuture
 * \brief 用于 Communicator 层的通信接口
 */
const int AbstractProtocol::defaultMaxExecuteTimes = 2;
const int AbstractProtocol::defaultTimeout = 1000;  // 1000 ms

AbstractProtocol::AbstractProtocol(QObject *parent) : QObject(parent)
{
    m_enabled  = true;
    m_priority = 0;

    m_cmdExecuteTime = defaultMaxExecuteTimes;
    m_cmdTimeout = defaultTimeout;
}

/*!
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
QByteArray AbstractProtocol::xorCODE(const QByteArray &ba)
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


void AbstractProtocol::startTiming()
{
    // do no thing
}


bool AbstractProtocol::enabled() const
{
    return m_enabled;
}

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

int AbstractProtocol::getCmdExecuteTime() const
{
    return m_cmdExecuteTime;
}

/*!
 * \brief 设置指令执行次数（0 表示当前协议将会一直等待指令回复，该值一般不使用，
 * 若在网络不畅时可以使用，即等待超时时间将会被忽略）
 *
 * -1 means waitting util the command is replied
 * (so waitTime will be ignored but it still reports FAILED)
 * 0 means that command won't be repeated
 */
void AbstractProtocol::setCmdExecuteTime(int cmdExecuteTime)
{
    m_cmdExecuteTime = cmdExecuteTime;
}

int AbstractProtocol::getCmdTimeout() const
{
    return m_cmdTimeout;
}

void AbstractProtocol::setCmdTimeout(int cmdTimeout)
{
    m_cmdTimeout = cmdTimeout;
}

// ============================

/*!
 * \fn AbstractProtocol::clearCmdCount
 * \brief 清空命令计数器
 */

/*!
 * \fn AbstractProtocol::skipCurrentCmd(const QByteArray &cmd = QByteArray())
 * 通知通讯器跳过当前查询(查询成功)
 */

/*!
 * \fn void AbstractProtocol::cmdAdded()
 *  新增加了一条指令
 */

/*!
 * \fn void AbstractProtocol::stopRemainCmd()
 * 停止查询操作
 */

/*!
 * \fn bool AbstractProtocol::processData(const QByteArray &data, const int index=0)
 * 通讯器接收到数据时将数据转交给协议进行处理(Communicator -> Protocol)
 * @return  true 表示该协议需要独享该数据，所以数据不应该传给下一个协议；
 *          true means the protocol need to process the data exclusively,
 *          so the data should not be passed to next protocol any more;
 *          false means not;
 */

/*!
 * \fn void AbstractProtocol::lastQueryFailed()
 * 计时器超时后调用该方法，表明查询失败
 */

//=====================
