#ifndef COMMGLOBAL_H
#define COMMGLOBAL_H

#include "commfactory.h"
#include "commmanager.h"

/**
 * 提供给其它组件使用。
 */
namespace Comm {
    extern CommFactory *factory;
    extern CommManager *manager;
    void BC_DLL_EXPORT init ();
    void BC_DLL_EXPORT destroy();

    void BC_DLL_EXPORT test(CommManager* m);
    void BC_DLL_EXPORT initManager(CommManager *manager);
}

#endif // COMMGLOBAL_H
