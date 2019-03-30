#include "BCPrecompiled.h"

#ifdef Q_OS_WIN
#ifndef _BC_STATIC_BUILD

#include <windows.h>

extern "C"
int WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpReservedt */)
{
   Q_UNUSED(hInstance);
   Q_UNUSED(dwReason);

#ifndef _BC_NO_TRACE_DLL_ATTACH_DETACH
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:   ::OutputDebugStringA("BasicComm ---> DLL_PROCESS_ATTACH\n");  break;
      case DLL_PROCESS_DETACH:   ::OutputDebugStringA("BasicComm ---> DLL_PROCESS_DETACH\n");  break;
      case DLL_THREAD_ATTACH:    ::OutputDebugStringA("BasicComm ---> DLL_THREAD_ATTACH\n");   break;
      case DLL_THREAD_DETACH:    ::OutputDebugStringA("BasicComm ---> DLL_THREAD_DETACH\n");   break;
   }
#endif // _BC_NO_TRACE_DLL_ATTACH_DETACH

   return 1;
}

#endif // _BC_STATIC_BUILD
#endif // Q_OS_WIN
