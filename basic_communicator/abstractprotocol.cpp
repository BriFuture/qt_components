#include "abstractprotocol.h"

/*!
 * \class AbstractProtocol
 * \ingroup comm
 * \author BriFuture
 * \brief 用于 Communicator 层的通信接口
 */
AbstractProtocol::AbstractProtocol(QObject *parent) : QObject(parent)
{
    m_enabled  = true;
    m_priority = 0;
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

bool AbstractProtocol::enabled() const
{
    return m_enabled;
}

void AbstractProtocol::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

int AbstractProtocol::priority() const
{
    return m_priority;
}

void AbstractProtocol::setPriority(int priority)
{
    m_priority = priority;
}

/*!
  * \fn AbstractProtocol::clearRemainCmd()
  * \brief 清空等待查询的命令
  */
