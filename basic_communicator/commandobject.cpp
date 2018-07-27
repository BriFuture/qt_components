#include "commandobject.h"
/*!
 * \class CommandObject
 * \ingroup comm
 * \author BriFuture
 * \date  2018.07.26
 * \brief CommandObject::CommandObject
 * \param parent
 */

/*!
 * \enum CommandObject::State
 * 命令的执行状态
 * \list
 *   \li Command::Waiting 表示该命令在队列中等待查询
 *   \li Command::Executing 表示该命令位于队列首部，正在查询，此时与之相应的计时器启动计时
 *   \li Command::Pending 表示该命令暂时被挂起，计时器暂停计时
 *   \li Command::Timeout 表示该命令在一定时间内尚未接收到回复（即指令执行失败）
 *   \li Command::RightReplied 表示该命令已经收到正常回复
 *   \li Command::WrongReplied 表示该命令已经收到回复，但回复并不与该指令对应（目前无法判断）
 * \endlist
 */

CommandObject::CommandObject(AbstractProtocol *parent) : QObject(parent)
{
    maxExecuteTimes = AbstractProtocol::defaultMaxExecuteTimes;
    timeout = AbstractProtocol::defaultTimeout;
}

AbstractProtocol *CommandObject::protocolBelongs()
{
    return static_cast<AbstractProtocol *>(parent());
}

int CommandObject::getMaxExecuteTimes() const
{
    return maxExecuteTimes;
}

void CommandObject::setMaxExecuteTimes(int value)
{
    maxExecuteTimes = value;
}

int CommandObject::getTimeout() const
{
    return timeout;
}

void CommandObject::setTimeout(int value)
{
    timeout = value;
}

QByteArray CommandObject::contents() const
{
    return m_contents;
}

void CommandObject::setContents(const QByteArray &value)
{
    m_contents = value;
}

CommandObject::State CommandObject::state() const
{
    return m_state;
}

void CommandObject::setState(const State &value)
{
    stateChanged( value );
    m_state = value;
}

QByteArray CommandObject::reply() const
{
    return m_reply;
}

void CommandObject::setReply(const QByteArray &value)
{
    m_reply = value;
}
