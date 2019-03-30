#include "commglobal.h"
#include <QMetaType>
#include <QDebug>

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

void Comm::initManager(CommManager *manager)
{
    if( inited ) {
        qInfo() << "[BasicComm] CommManager has been initiated.";
        return;
    }
    if(manager == 0) {
        qWarning() << "[BasicComm] Passed in CommManager is nullptr.";
        return;
    }
    qDebug() << "[TODO BasicComm] init manager";
}

void Comm::test(CommManager *m)
{
    qDebug() << "This is a test function, passed in: " << m;
}
