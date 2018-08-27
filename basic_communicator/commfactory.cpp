#include "commfactory.h"
#include "comfullduplex.h"
#include "comhalfduplex.h"
#include "commnetwork.h"
#include "virtualcom.h"
#include "commglobal.h"

#include <QDebug>

/*!
 * \class CommFactory
 * \ingroup comm
 * \author BriFuture
 * \date 2018.07.19
 *
 * \brief The CommFactory class
 * 通讯器工厂类，增加抽象通讯类的子类时，在工厂类中添加相应的模式和
 * 构造实现类的例化。
 * 主要作用是构造 AbstractComm 的实例以及返回相应的属性
 *
 * Factory class, when adding new subclass of BaseComm,
 * add the proper mode and realization into this class.
 *
 * 使用 Comm::factory 或 CommFactory::defaultFactory 获得默认的工厂对象指针
 *
 * example:
 * \code
 *   CommFactory *cf1 = Comm::factory;
 *   CommFactory *cf2 = CommFactory::defaultFactory();
 *   qDebug() << (cf1 == cf2); // true
 *   AbstractComm *chd = cf1->createComm("CommHalfDuplex");  // chd is an instance of ComHalfDuplex
 * \endcode
 *
 * 使用工厂类时不要直接构造新的工厂对象，否则没有通讯器的模版工厂将只能构造 ComFullDuplex 的对象
 *
 * example:
 * \code
 *   CommFactory *cf3 = new CommFactory;
 *   AbstractComm *ac = cf3->createComm("CommHalfDuplex");  // ac is an instance of ComFullDuplex instead of ComHalfDuplex
 * \endcode
 *
 * 在其他组件中添加新的通讯器示例：
 * \code
 *   Comm::factory->add(ImplComm::staticMetaObject, ImplComm::commInfo);
 *   ...
 *   Comm::manager->resetMode("ImplComm");  // Communicator has been changed to ImplComm
 * \endcode
 */

const QString CommFactory::defaultCommName = ComFullDuplex::staticMetaObject.className();
#define MyAddComm(COMM, INFO)  addComm(COMM::staticMetaObject, INFO)


CommFactory::CommFactory(QObject *parent) : QObject(parent)
{
    CommInfo info;
    info.type = QObject::tr("SerialPort");
    info.desc = QObject::tr("Full Duplex Mode");
    info.isHalfDuplex = false;
    MyAddComm(ComFullDuplex, info);
    info.type = QObject::tr("SerialPort");
    info.desc = QObject::tr("Half Duplex Mode");
    info.isHalfDuplex = true;
    MyAddComm(ComHalfDuplex, info);
    info.type = QObject::tr("Network");
    info.desc = QObject::tr("Network Mode");
    info.isHalfDuplex = false;
    MyAddComm(CommNetwork, info);
    info.type = QObject::tr("Simulator");
    info.desc = QObject::tr("Simulator Test Mode");
    info.isHalfDuplex = false;
    MyAddComm(VirtualCom, info);
}

CommFactory::CommFactory(const CommFactory &cf)
{
    this->m_commMap = cf.m_commMap;
    this->m_usableComm = cf.usableComm();
}

void CommFactory::operator =(const CommFactory &cf)
{
    this->m_commMap = cf.m_commMap;
    this->m_usableComm = cf.usableComm();
}

/*!
 * \brief CommFactory::defaultFactory
 * \return 返回默认的工程对象
 * \note 也可以使用 Comm::factory 对象指针作为默认的工厂
 *
 * \code
 *   CommFactory *f = CommFactory::defaultFactory();  // f is a pointer to default Factory
 * \endcode
 */
CommFactory *CommFactory::defaultFactory()
{
    static CommFactory *fac = 0;
    if( fac == 0 ) {
        fac = new CommFactory;
    }
    return fac;
}

/*!
 * \brief 根据 className 例化相应的通讯实现类
 *
 * 目前支持的通讯类模式有
 * \list
 *   \li 串口全双工模式
 *   \li 串口半双工模式
 *   \li 网络模式（全双工）
 *   \li 测试模式
 * \endlist
 *
 * className 对应的是 AbstractComm 子类的类名，根据类名构造子类对象，
 * 若子类对象此前已经构造过，那么将会从保存的列表中直接取出子类对象的指针
 *
 * \code
 *   CommFactory  *cf  = Comm::factory;
 *   AbstractComm *ac  = cf->createComm("ComFullDuplex");
 *   AbstractComm *ac2 = cf->createComm( "ComHalfDuplex" );
 *   AbstractComm *ac3 = cf->createComm("ComFullDuplex");
 *   Q_ASSERT( ac  != 0 );   // true
 *   Q_ASSERT( ac2 != 0 );   // true
 *   Q_ASSERT( ac2 != ac );  // true
 *   Q_ASSERT( ac3 != 0 );   // true
 *   Q_ASSERT( ac3 == ac );  // true
 *   Q_ASSERT( ac3 != ac2 ); // true
 * \endcode
 *
 * \sa Comm
 */
AbstractComm *CommFactory::createComm(const QString &className)
{
    AbstractComm *comm;
    foreach (AbstractComm *ac, m_readyComm) {
        if( ac->metaObject()->className() == className ) {
            comm = ac;
            return comm;
        }
    }
    foreach(QMetaObject mo, m_usableComm) {
        if( mo.className() == className ) {
            comm = static_cast<AbstractComm *>(mo.newInstance());
            m_readyComm.append( comm );
            break;
        }
    }

    if( comm == 0 ) {
        comm = new ComFullDuplex;
    }
    return comm;
}

/*!
 * \brief CommFactory::addComm
 * \param metaObject
 * \param info
 * 添加一个可用的 AbstractComm 元对象，存放相应的信息，用于构造 AbstractComm 实现类
 *
 *  example:
 * \code
 *   CommFactory factory;
 *   factory.addComm(ComFullDuplex::staticMetaObject, ComFullDuplex::commInfo);  // factory can generate an Object points to ComFullDuplex
 *   factory.createComm("CommFactory");
 * \endcode
 *
 * \sa CommFactory::createComm(const QString &)
 */
void CommFactory::addComm(const QMetaObject &metaObject, const CommInfo &info)
{
    QString className(metaObject.className());
    if( m_commMap.contains(className) ) {
        return;
    }
    m_usableComm.append(metaObject);
    m_commMap.insert( className, info );
}

/*!
 * \brief 提供接口用于显示可用的通讯模式
 * 返回的 comm 对象用于界面显示
 * m_commMap 包含的键值对中，键位类名，值为对应的 commInfo
 *
 * example:
 * 假设 m_commMap 中只存有 ComFullDuplex 这个类
 * \code
 *   QMapIterator<QString, CommInfo> it(m_commMap);
 *   while(it.hasNext()) {
 *       it.next();
 *       qDebug() << it.key();           // output: "ComFullDuplex"
 *       qDebug() << it.value().type;    // output: "SerialPort"
 *       qDebug() << it.value().desc;    // output: "Full Duplex"
 *   }
 *
 * \endcode
 */
QList<CommInfo> CommFactory::commInfo()
{
    return m_commMap.values();
}


/*!
 * \brief 根据通讯器的类型 type（SerialPort，Network） 判断其是否支持半双工模式
 * 用于界面，方便选择具体的通讯器
 *
 * example:
 * \code
 *   CommFactory factory;
 *   factory.addComm(ComHalfDuplex::staticMetaObject, ComHalfDuplex::commInfo);  // type: SerialPort
 *   bool shd = factory.supportHalfDuplex("SerialPort");   // true
 *   bool shd2 = factory.supportHalfDuplex("Network");     // false
 * \endcode
 */
bool CommFactory::supportHalfDuplex(const QString &type)
{
    QMapIterator<QString, CommInfo> it(m_commMap);

    while( it.hasNext() ) {
        it.next();
        const CommInfo ci = it.value();
        if( ci.type == type && ci.isHalfDuplex ) {
            return true;
        }
    }
    return false;
}

/*!
 * \brief 根据通讯器类型和工作模式（是否为半双工）返回相应的通讯器类名
 * 类名在构造通讯器时中用到
 *
 * example:
 * \code
 *   factory = new ComFactory;
 *   factory.addComm(ComHalfDuplex::staticMetaObject, ComHalfDuplex::commInfo);  // type: SerialPort, mode: half duplex
 *   QString className = factory.commClassName("SerialPort", true); // ComHalfDuplex
 * \endcode
 */
const QString CommFactory::commClassName(const QString &type, const bool halfDuplex)
{
    QMapIterator<QString, CommInfo> it(m_commMap);

    while( it.hasNext() ) {
        it.next();
        const CommInfo ci = it.value();
        if( ci.type == type && halfDuplex == ci.isHalfDuplex ) {
            return it.key();
        }
    }

    return QString();
}

/*!
 * \brief CommFactory::usableComm
 * \return 返回可用的元对象列表，用于动态构造对象
 */
QVector<QMetaObject> CommFactory::usableComm() const
{
    return m_usableComm;
}

///*!
// * \brief CommFactory::typeName
// * \param className
// * \return 如传入的是 ComFullDuplex，则返回 SerialPort
// */
//const QString CommFactory::typeName(const QString &className)
//{
//    foreach (CommMode m, comm) {
//        if( m.fdClassName == className || m.hdClassName == className ) {
//            return m.type;
//        }
//    }
//    return QString();
//}
