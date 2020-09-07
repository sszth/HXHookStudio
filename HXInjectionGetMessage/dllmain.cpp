// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <stdio.h>
BOOL APIENTRY DllMain( HMODULE hInstDll,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//// ok open dlg success
		//TCHAR szqq[MAX_PATH] = TEXT("hun");
		//MessageBox(GetActiveWindow(), szqq, szqq, MB_OK);
		//break;
        char szBuf[MAX_PATH * 100] = { 0 };

        PBYTE pb = NULL;
        MEMORY_BASIC_INFORMATION mbi;
        while (VirtualQuery(pb, &mbi, sizeof(mbi)) == sizeof(mbi)) {

            int nLen;
            char szModName[MAX_PATH];

            if (mbi.State == MEM_FREE)
                mbi.AllocationBase = mbi.BaseAddress;

            if ((mbi.AllocationBase == hInstDll) ||
                (mbi.AllocationBase != mbi.BaseAddress) ||
                (mbi.AllocationBase == NULL)) {
                // Do not add the module name to the list
                // if any of the following is true:
                // 1. If this region contains this DLL
                // 2. If this block is NOT the beginning of a region
                // 3. If the address is NULL
                nLen = 0;
            }
            else {
                nLen = GetModuleFileNameA((HINSTANCE)mbi.AllocationBase,
                    szModName, __crt_countof(szModName));
            }

            if (nLen > 0) {
                wsprintfA(strchr(szBuf, 0), "\n%p-%s",
                    mbi.AllocationBase, szModName);
            }

            pb += mbi.RegionSize;
        }

        // NOTE: Normally, you should not display a message box in DllMain
        // due to the loader lock described in Chapter 20. However, to keep
        // this sample application simple, I am violating this rule.
        PCSTR szMsg = &szBuf[1];
        char szTitle[MAX_PATH];
        GetModuleFileNameA(NULL, szTitle, __crt_countof(szTitle));
        MessageBoxA(GetActiveWindow(), szMsg, szTitle, MB_OK);
        break;

    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    //    break;
    }
    return TRUE;
}

