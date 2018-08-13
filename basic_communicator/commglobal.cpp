#include "commglobal.h"
#include <QMetaType>
#include "comfullduplex.h"
#include "comhalfduplex.h"
#include "commnetwork.h"
#include "virtualcom.h"

#define MyAddComm(COMM, INFO)  factory->addComm(COMM::staticMetaObject, INFO)

CommFactory * Comm::factory = 0;
CommManager * Comm::manager = 0;

bool inited = false;

/*!
 * \namespace Comm
 * \brief Comm::init
 * 初始化通讯层组件
 * \note 在使用通讯器时，必须先调用初始化方法，否则程序将会崩溃
 *
 * example:
 * \code
 *   QCoreApplication a(argc, argv);
 *   Comm::init();
 *
 *   ...
 *
 *   int rescode = a.exec();
 *   Comm::destroy();
 *   return rescode;
 * \endcode
 */
void Comm::init() {
    if( inited ) {
        return;
    }
    qRegisterMetaType<CommState>();

    factory = CommFactory::defaultFactory();

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

    manager = new CommManager;
    inited = true;
}

/*!
 * \namespace Comm
 * \brief Comm::destroy
 * 销毁通讯层组件
 */
void Comm::destroy()
{
    manager->removeAllProtocol();
    manager->stopAllQuery(); // 停止所有查询
    manager->deleteLater();
    factory->deleteLater();
}

/*!
 * \group comm
 * \title Communication Layer Related
 * 通讯层的相关类
 */
