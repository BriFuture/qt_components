#ifndef BASICCOMM_GLOBAL_H
#define BASICCOMM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BASICCOMM_LIBRARY)
#  define BC_DLL_EXPORT Q_DECL_EXPORT
#else
#  define BC_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // BASICCOMM_GLOBAL_H
