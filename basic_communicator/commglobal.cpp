﻿#include "commglobal.h"
#include <QMetaType>
#include "comfullduplex.h"
#include "comhalfduplex.h"
#include "commnetwork.h"
#include "virtualcom.h"

#define MyAddComm(COMM) factory->addComm(COMM::staticMetaObject, COMM::commInfo)

CommFactory * Comm::factory = 0;
CommManager * Comm::manager = 0;

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
    static bool inited = false;
    if( inited ) {
        return;
    }
    qRegisterMetaType<CommState>();

    factory = CommFactory::defaultFactory();
    MyAddComm(ComFullDuplex);
    MyAddComm(ComHalfDuplex);
    MyAddComm(CommNetwork);
    MyAddComm(VirtualCom);

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
    factory->deleteLater();
    manager->deleteLater();
}

/*!
 * \group comm
 * \title Communication Layer Related
 * 通讯层的相关类
 */