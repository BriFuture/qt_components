#ifndef COMMGLOBAL_H
#define COMMGLOBAL_H

#include <QMap>
#include <QVector>
#include "commfactory.h"
#include "commmanager.h"

/**
 * 提供给其它组件使用。
 */
namespace Comm {
    extern CommFactory *factory;
    extern CommManager *manager;
    void init ();
    void destroy();
}


#endif // COMMGLOBAL_H
